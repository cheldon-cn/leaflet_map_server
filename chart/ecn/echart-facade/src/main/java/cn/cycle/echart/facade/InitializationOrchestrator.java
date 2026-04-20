package cn.cycle.echart.facade;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

/**
 * 初始化编排器。
 * 
 * <p>管理应用程序启动时的服务初始化顺序和依赖关系。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class InitializationOrchestrator {

    private final List<InitializationTask> tasks;
    private final Map<String, InitializationTask> taskMap;
    private final List<InitializationListener> listeners;
    private final ExecutorService executor;
    
    private boolean initialized = false;
    private int timeoutSeconds = 60;

    public InitializationOrchestrator() {
        this.tasks = new ArrayList<>();
        this.taskMap = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
        this.executor = Executors.newCachedThreadPool();
    }

    public void registerTask(String id, String name, Runnable task, int priority) {
        registerTask(id, name, task, priority, new ArrayList<>());
    }

    public void registerTask(String id, String name, Runnable task, int priority, 
                            List<String> dependencies) {
        Objects.requireNonNull(id, "id cannot be null");
        Objects.requireNonNull(task, "task cannot be null");

        InitializationTask initTask = new InitializationTask(id, name, task, priority, dependencies);
        
        if (taskMap.containsKey(id)) {
            throw new IllegalArgumentException("Task already registered: " + id);
        }

        tasks.add(initTask);
        taskMap.put(id, initTask);
    }

    public void initialize() throws InitializationException {
        if (initialized) {
            return;
        }

        notifyInitializationStarted();

        List<InitializationTask> sortedTasks = resolveDependencies();
        
        Map<String, Boolean> completed = new ConcurrentHashMap<>();
        CountDownLatch latch = new CountDownLatch(sortedTasks.size());

        for (InitializationTask task : sortedTasks) {
            executor.submit(() -> {
                try {
                    waitForDependencies(task, completed);
                    
                    notifyTaskStarted(task);
                    
                    long startTime = System.currentTimeMillis();
                    task.getTask().run();
                    long duration = System.currentTimeMillis() - startTime;
                    
                    completed.put(task.getId(), true);
                    notifyTaskCompleted(task, duration);
                    
                } catch (Exception e) {
                    notifyTaskFailed(task, e);
                } finally {
                    latch.countDown();
                }
            });
        }

        try {
            boolean success = latch.await(timeoutSeconds, TimeUnit.SECONDS);
            if (!success) {
                throw new InitializationException("Initialization timed out after " + timeoutSeconds + " seconds");
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            throw new InitializationException("Initialization interrupted", e);
        }

        initialized = true;
        notifyInitializationCompleted();
    }

    private List<InitializationTask> resolveDependencies() throws InitializationException {
        List<InitializationTask> result = new ArrayList<>();
        Map<String, Integer> inDegree = new HashMap<>();
        Map<String, List<String>> graph = new HashMap<>();

        for (InitializationTask task : tasks) {
            inDegree.put(task.getId(), task.getDependencies().size());
            graph.put(task.getId(), new ArrayList<>());
        }

        for (InitializationTask task : tasks) {
            for (String dep : task.getDependencies()) {
                if (!taskMap.containsKey(dep)) {
                    throw new InitializationException("Unknown dependency: " + dep + " for task: " + task.getId());
                }
                graph.get(dep).add(task.getId());
            }
        }

        List<String> queue = new ArrayList<>();
        for (Map.Entry<String, Integer> entry : inDegree.entrySet()) {
            if (entry.getValue() == 0) {
                queue.add(entry.getKey());
            }
        }

        while (!queue.isEmpty()) {
            String taskId = queue.remove(0);
            result.add(taskMap.get(taskId));

            for (String dependent : graph.get(taskId)) {
                int newDegree = inDegree.get(dependent) - 1;
                inDegree.put(dependent, newDegree);
                if (newDegree == 0) {
                    queue.add(dependent);
                }
            }
        }

        if (result.size() != tasks.size()) {
            throw new InitializationException("Circular dependency detected in initialization tasks");
        }

        result.sort(Comparator.comparingInt(InitializationTask::getPriority));

        return result;
    }

    private void waitForDependencies(InitializationTask task, Map<String, Boolean> completed) 
            throws InterruptedException {
        for (String dep : task.getDependencies()) {
            while (!completed.getOrDefault(dep, false)) {
                Thread.sleep(50);
            }
        }
    }

    public void shutdown() {
        executor.shutdown();
        try {
            if (!executor.awaitTermination(10, TimeUnit.SECONDS)) {
                executor.shutdownNow();
            }
        } catch (InterruptedException e) {
            executor.shutdownNow();
            Thread.currentThread().interrupt();
        }
    }

    public void setTimeoutSeconds(int seconds) {
        this.timeoutSeconds = seconds;
    }

    public int getTimeoutSeconds() {
        return timeoutSeconds;
    }

    public boolean isInitialized() {
        return initialized;
    }

    public List<InitializationTask> getTasks() {
        return new ArrayList<>(tasks);
    }

    public void addListener(InitializationListener listener) {
        listeners.add(listener);
    }

    public void removeListener(InitializationListener listener) {
        listeners.remove(listener);
    }

    private void notifyInitializationStarted() {
        for (InitializationListener listener : listeners) {
            listener.onInitializationStarted();
        }
    }

    private void notifyInitializationCompleted() {
        for (InitializationListener listener : listeners) {
            listener.onInitializationCompleted();
        }
    }

    private void notifyTaskStarted(InitializationTask task) {
        for (InitializationListener listener : listeners) {
            listener.onTaskStarted(task);
        }
    }

    private void notifyTaskCompleted(InitializationTask task, long durationMs) {
        for (InitializationListener listener : listeners) {
            listener.onTaskCompleted(task, durationMs);
        }
    }

    private void notifyTaskFailed(InitializationTask task, Exception e) {
        for (InitializationListener listener : listeners) {
            listener.onTaskFailed(task, e);
        }
    }

    public static class InitializationTask {
        private final String id;
        private final String name;
        private final Runnable task;
        private final int priority;
        private final List<String> dependencies;

        public InitializationTask(String id, String name, Runnable task, int priority, 
                                  List<String> dependencies) {
            this.id = id;
            this.name = name;
            this.task = task;
            this.priority = priority;
            this.dependencies = new ArrayList<>(dependencies);
        }

        public String getId() {
            return id;
        }

        public String getName() {
            return name;
        }

        public Runnable getTask() {
            return task;
        }

        public int getPriority() {
            return priority;
        }

        public List<String> getDependencies() {
            return new ArrayList<>(dependencies);
        }
    }

    public interface InitializationListener {
        void onInitializationStarted();
        void onInitializationCompleted();
        void onTaskStarted(InitializationTask task);
        void onTaskCompleted(InitializationTask task, long durationMs);
        void onTaskFailed(InitializationTask task, Exception e);
    }

    public static class InitializationException extends Exception {
        public InitializationException(String message) {
            super(message);
        }

        public InitializationException(String message, Throwable cause) {
            super(message, cause);
        }
    }
}
