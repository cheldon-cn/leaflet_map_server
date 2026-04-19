package cn.cycle.echart.route;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventBus;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 航线管理器。
 * 
 * <p>管理航线的创建、删除、查询和监控。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class RouteManager {

    private final EventBus eventBus;
    private final Map<String, Route> routes;
    private Route activeRoute;
    private final List<RouteListener> listeners;

    public RouteManager(EventBus eventBus) {
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
        this.routes = new ConcurrentHashMap<>();
        this.activeRoute = null;
        this.listeners = new ArrayList<>();
    }

    /**
     * 创建新航线。
     * 
     * @param name 航线名称
     * @return 创建的航线
     */
    public Route createRoute(String name) {
        Route route = new Route(name);
        routes.put(route.getId(), route);
        notifyRouteCreated(route);
        return route;
    }

    /**
     * 删除航线。
     * 
     * @param routeId 航线ID
     * @return 是否成功删除
     */
    public boolean deleteRoute(String routeId) {
        Route route = routes.remove(routeId);
        if (route != null) {
            if (activeRoute != null && activeRoute.getId().equals(routeId)) {
                activeRoute = null;
            }
            notifyRouteDeleted(route);
            return true;
        }
        return false;
    }

    /**
     * 获取航线。
     * 
     * @param routeId 航线ID
     * @return 航线
     */
    public Route getRoute(String routeId) {
        return routes.get(routeId);
    }

    /**
     * 获取所有航线。
     * 
     * @return 航线列表
     */
    public List<Route> getAllRoutes() {
        return new ArrayList<>(routes.values());
    }

    /**
     * 获取航线数量。
     * 
     * @return 航线数量
     */
    public int getRouteCount() {
        return routes.size();
    }

    /**
     * 设置活动航线。
     * 
     * @param routeId 航线ID
     * @return 是否成功设置
     */
    public boolean setActiveRoute(String routeId) {
        Route route = routes.get(routeId);
        if (route != null) {
            Route oldActive = activeRoute;
            activeRoute = route;
            notifyActiveRouteChanged(oldActive, activeRoute);
            return true;
        }
        return false;
    }

    /**
     * 获取活动航线。
     * 
     * @return 活动航线
     */
    public Route getActiveRoute() {
        return activeRoute;
    }

    /**
     * 清除活动航线。
     */
    public void clearActiveRoute() {
        if (activeRoute != null) {
            Route oldActive = activeRoute;
            activeRoute = null;
            notifyActiveRouteChanged(oldActive, null);
        }
    }

    /**
     * 更新航线。
     * 
     * @param route 航线
     */
    public void updateRoute(Route route) {
        Objects.requireNonNull(route, "route cannot be null");
        if (routes.containsKey(route.getId())) {
            notifyRouteUpdated(route);
        }
    }

    /**
     * 添加航线监听器。
     * 
     * @param listener 监听器
     */
    public void addRouteListener(RouteListener listener) {
        listeners.add(listener);
    }

    /**
     * 移除航线监听器。
     * 
     * @param listener 监听器
     */
    public void removeRouteListener(RouteListener listener) {
        listeners.remove(listener);
    }

    private void notifyRouteCreated(Route route) {
        eventBus.publish(new AppEvent(this, AppEventType.ROUTE_CREATED, route));
        
        for (RouteListener listener : listeners) {
            listener.onRouteCreated(route);
        }
    }

    private void notifyRouteDeleted(Route route) {
        eventBus.publish(new AppEvent(this, AppEventType.ROUTE_DELETED, route));
        
        for (RouteListener listener : listeners) {
            listener.onRouteDeleted(route);
        }
    }

    private void notifyRouteUpdated(Route route) {
        eventBus.publish(new AppEvent(this, AppEventType.ROUTE_MODIFIED, route));
        
        for (RouteListener listener : listeners) {
            listener.onRouteUpdated(route);
        }
    }

    private void notifyActiveRouteChanged(Route oldRoute, Route newRoute) {
        for (RouteListener listener : listeners) {
            listener.onActiveRouteChanged(oldRoute, newRoute);
        }
    }

    /**
     * 航线监听器接口。
     */
    public interface RouteListener {
        void onRouteCreated(Route route);
        void onRouteDeleted(Route route);
        void onRouteUpdated(Route route);
        void onActiveRouteChanged(Route oldRoute, Route newRoute);
    }
}
