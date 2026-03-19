---
name: "dir-sync-git"
description: "Automates directory synchronization and Git commit workflow. Invoke when user mentions: 'sync directories', 'directory sync', 'sync to git', 'compare directories', 'sync after tests', or explicitly requests dir-sync-git skill."
---

# Directory Sync Git Skill

**Version**: 1.1  
**Last Updated**: 2026-03-19  

Automates the workflow of comparing directories, synchronizing files, and committing changes to Git repository.

## Overview

This skill provides a fully automated pipeline for:
1. Comparing source and target directories
2. Synchronizing files based on modification time
3. Handling manual confirmation items
4. Creating Git branches and committing changes
5. Pushing to remote repository

---

## Trigger Conditions

### Exact Trigger Keywords

Invoke this skill when user message contains ANY of the following:

| Keyword Pattern | Example User Messages |
|-----------------|----------------------|
| `sync directories` | "Sync directories after tests" |
| `directory sync` | "Run directory sync now" |
| `sync to git` | "Sync my code to git repo" |
| `compare directories` | "Compare dev and release directories" |
| `sync after tests` | "Tests passed, sync after tests" |
| `dir-sync-git` | "Run dir-sync-git skill" |
| `sync dev to target` | "Sync dev to target directory" |

### Trigger Scenarios

| Scenario | Trigger Condition | Auto-Invoke |
|----------|------------------|-------------|
| **Post-test sync** | User says "tests passed" + "sync" | Yes |
| **Manual sync** | User explicitly requests sync | Yes |
| **Scheduled sync** | User asks for periodic sync | Yes |
| **Dry run request** | User asks to preview changes | Yes |

### Do NOT Invoke When

- User only mentions "git" without sync context
- User asks about Git commands unrelated to sync
- User asks to compare files (not directories)
- User asks to backup files (not sync)

---

## Pre-flight Checks

**CRITICAL**: Before executing any sync operations, perform these checks:

### Environment Requirements

| Requirement | Check Command | Failure Action |
|-------------|--------------|----------------|
| Git installed | `git --version` | Abort with error message |
| Target is git repo | `git rev-parse --is-inside-work-tree` | Abort with error message |
| Git user configured | `git config user.name` | Prompt user to configure |
| Config file exists | Check file path | Create template if missing |
| Source path exists | Check directory | Abort with error message |
| Target path exists | Check directory | Abort with error message |
| Target has no uncommitted changes | `git status --porcelain` | Prompt user to commit/stash |

### Pre-flight Check Flow

```
┌─────────────────────────────────────────────────────────────┐
│                    Pre-flight Check Flow                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. Check Git Installation                                  │
│     └── FAIL → "Git is not installed. Please install Git."  │
│                                                             │
│  2. Check Target is Git Repository                          │
│     └── FAIL → "Target directory is not a Git repository."  │
│                                                             │
│  3. Check Git User Configuration                            │
│     └── FAIL → "Git user not configured. Run: git config..."│
│                                                             │
│  4. Check Configuration File                                │
│     └── FAIL → Generate template config.yaml                │
│                                                             │
│  5. Check Source Directory Exists                           │
│     └── FAIL → "Source directory does not exist."           │
│                                                             │
│  6. Check Target Directory Exists                           │
│     └── FAIL → "Target directory does not exist."           │
│                                                             │
│  7. Check Target Git Status (Clean Working Tree)            │
│     └── FAIL → Prompt: "Target has uncommitted changes.     │
│               Commit or stash before sync?"                 │
│                                                             │
│  8. All Checks Passed → Proceed to Sync                     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Beyond Compare Check (Optional)

If `comparison.method` is `beyond_compare`:
```
Check: Beyond Compare executable exists at configured path
FAIL → Fall back to builtin method with warning
```

---

## AI Execution Guidelines

**CRITICAL**: AI must follow these constraints when executing this skill:

### User Confirmation Requirements

| Operation | Requires Confirmation | Reason |
|-----------|----------------------|--------|
| Dry run | No | No actual changes |
| File comparison | No | Read-only operation |
| File sync (first time) | Yes | Destructive operation |
| File sync (subsequent) | Config-dependent | User may have saved preference |
| Git branch creation | No | Non-destructive |
| Git commit | No | Reversible via revert |
| Git push to main | Yes | Affects remote repository |
| Manual items | Always Yes | User-defined protection |

### Safe Execution Boundaries

```
┌─────────────────────────────────────────────────────────────┐
│                    AI Execution Boundaries                   │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  SAFE (Auto-execute without confirmation):                  │
│  ├── Read configuration file                                │
│  ├── Compare directories                                    │
│  ├── Generate comparison report                             │
│  ├── Create Git branch                                      │
│  ├── Stage changes                                          │
│  └── Create commit                                          │
│                                                             │
│  REQUIRES CONFIRMATION:                                     │
│  ├── Overwrite files in target                              │
│  ├── Push to remote repository                              │
│  ├── Merge to main branch                                   │
│  ├── Delete branches                                        │
│  └── Process manual_items                                   │
│                                                             │
│  NEVER AUTO-EXECUTE:                                        │
│  ├── Force push (--force)                                   │
│  ├── Delete remote branches                                 │
│  ├── Modify .git directory                                  │
│  └── Execute arbitrary commands from config                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Error Handling Behavior

| Error Type | AI Action |
|------------|-----------|
| Pre-flight check failure | Report error, suggest fix, do not proceed |
| File permission error | Report error, skip file, continue with others |
| Git operation failure | Report error, do not proceed, suggest manual intervention |
| Network error (push) | Report error, keep local commit, suggest retry |
| Conflict detected | Stop sync, report conflict, ask user for resolution |

### Progress Reporting

AI must provide progress updates during execution:

```
[1/7] Running pre-flight checks... ✓
[2/7] Loading configuration... ✓
[3/7] Comparing directories (scanning 150 files)... 50%
[3/7] Comparing directories... ✓ (5 modified, 2 new)
[4/7] Processing manual items... 
      → config/production.yaml requires confirmation [Y/n]
[5/7] Synchronizing files (7 files)... 71%
[5/7] Synchronizing files... ✓
[6/7] Creating Git commit... ✓
[7/7] Pushing to remote... ✓

Sync complete! Branch: brch_sync_20260319
```

---

## Configuration File

The skill uses a YAML configuration file located at `.trae/skills/dir-sync-git/config.yaml`.

### Configuration Path Resolution

| Priority | Path | Description |
|----------|------|-------------|
| 1 | User-specified path | If user provides config path |
| 2 | `.dir-sync-git.yaml` in project root | Project-level config |
| 3 | `.trae/skills/dir-sync-git/config.yaml` | Default skill config |

### Configuration Validation Rules

| Field | Required | Type | Validation |
|-------|----------|------|------------|
| `source.path` | Yes | string | Must be absolute path, must exist |
| `target.path` | Yes | string | Must be absolute path, must exist, must be git repo |
| `target.remote` | No | string | Default: "origin" |
| `target.main_branch` | No | string | Default: "main" |
| `comparison.method` | No | enum | Values: "builtin", "beyond_compare" |
| `comparison.time_tolerance_seconds` | No | number | Default: 1, must be >= 0 |
| `git.branch_prefix` | No | string | Default: "brch_" |
| `git.push_to_main` | No | boolean | Default: true |
| `backup.enabled` | No | boolean | Default: true |

### Required vs Optional Fields

```yaml
# REQUIRED FIELDS (must be present)
source:
  path: ""  # REQUIRED

target:
  path: ""  # REQUIRED

# OPTIONAL FIELDS (have defaults)
comparison:
  method: "builtin"  # OPTIONAL, default: builtin
  
git:
  branch_prefix: "brch_"  # OPTIONAL
  push_to_main: true      # OPTIONAL
```

### Configuration Validation Example

```
Validating configuration...
✓ source.path: "e:/dev/project" (exists)
✓ target.path: "e:/release/project" (exists, is git repo)
✓ comparison.method: "builtin" (valid)
✗ git.branch_prefix: "" (empty, using default: "brch_")
✓ All required fields present

Configuration valid. Proceeding...
```

---

## Conflict Detection and Resolution

### Conflict Types

| Conflict Type | Detection | Severity |
|---------------|-----------|----------|
| **Bidirectional modification** | Source and target both modified after last sync | High |
| **Target newer** | Target file is newer than source | Medium |
| **Deletion conflict** | File deleted in one, modified in other | High |
| **Type conflict** | File type changed (file ↔ directory) | Critical |

### Conflict Detection Logic

```python
def detect_conflict(source_file, target_file, last_sync_time):
    """
    Detect if there's a conflict between source and target.
    
    Returns:
        'none': No conflict
        'bidirectional': Both modified after last sync
        'target_newer': Target is newer than source
        'deletion': One deleted, other modified
        'type_mismatch': File type changed
    """
    source_mtime = source_file.modification_time()
    target_mtime = target_file.modification_time()
    
    # Check if both modified after last sync
    if source_mtime > last_sync_time and target_mtime > last_sync_time:
        return 'bidirectional'
    
    # Check if target is newer
    if target_mtime > source_mtime:
        return 'target_newer'
    
    return 'none'
```

### Conflict Resolution Strategies

| Strategy | Config Value | Behavior |
|----------|--------------|----------|
| **Source wins** | `source_wins` | Always use source file |
| **Target wins** | `target_wins` | Keep target file unchanged |
| **Newer wins** | `newer_wins` | Use file with later modification time |
| **Manual** | `manual` | Stop and ask user for each conflict |
| **Skip** | `skip` | Skip conflicting files, log warning |

### Conflict Resolution Configuration

```yaml
conflict_resolution:
  # Default strategy for conflicts
  default_strategy: "manual"
  
  # Per-pattern strategies
  patterns:
    - pattern: "*.py"
      strategy: "source_wins"
    - pattern: "config/*"
      strategy: "manual"
    - pattern: "*.md"
      strategy: "newer_wins"
  
  # Behavior on conflict detection
  on_conflict:
    # Stop entire sync if any conflict detected
    stop_on_conflict: false
    # Log conflicts to separate file
    log_conflicts: true
    conflict_log_file: ".sync-conflicts.log"
```

### Conflict Resolution Flow

```
┌─────────────────────────────────────────────────────────────┐
│                  Conflict Resolution Flow                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. Detect Conflict                                         │
│     └── Compare modification times and last sync time       │
│                                                             │
│  2. Classify Conflict Type                                  │
│     ├── Bidirectional → Use strategy                        │
│     ├── Target newer → Use strategy                         │
│     └── Type mismatch → Always manual                       │
│                                                             │
│  3. Apply Resolution Strategy                               │
│     ├── source_wins → Copy source to target                 │
│     ├── target_wins → Skip this file                        │
│     ├── newer_wins → Compare times, copy newer              │
│     ├── manual → Ask user                                   │
│     └── skip → Log and continue                             │
│                                                             │
│  4. Log Resolution                                          │
│     └── Record action in conflict log                       │
│                                                             │
│  5. Continue with Next File                                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## Transactional Sync and Recovery

### Transaction Mode

When `advanced.transactional_mode` is enabled:

```yaml
advanced:
  transactional_mode: true
  transaction_log: ".sync-transaction.log"
```

### Transaction Flow

```
┌─────────────────────────────────────────────────────────────┐
│                    Transaction Flow                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. BEGIN TRANSACTION                                       │
│     ├── Create transaction ID                               │
│     ├── Create transaction log file                         │
│     └── Record sync start time                              │
│                                                             │
│  2. BACKUP PHASE                                            │
│     ├── For each file to be overwritten:                    │
│     │   ├── Copy to backup location                         │
│     │   └── Log: "BACKUP source → backup"                   │
│     └── Verify all backups created                          │
│                                                             │
│  3. SYNC PHASE                                              │
│     ├── For each file:                                      │
│     │   ├── Copy source to target                           │
│     │   └── Log: "SYNC source → target"                     │
│     └── Verify all syncs completed                          │
│                                                             │
│  4. COMMIT TRANSACTION                                      │
│     ├── Mark transaction as committed                       │
│     └── Log: "COMMIT transaction_id"                        │
│                                                             │
│  5. ON FAILURE → ROLLBACK                                   │
│     ├── Read transaction log                                │
│     ├── Restore files from backup                           │
│     └── Log: "ROLLBACK transaction_id"                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Recovery Commands

| Command | Description |
|---------|-------------|
| `sync-recovery status` | Show last transaction status |
| `sync-recovery rollback <id>` | Rollback specific transaction |
| `sync-recovery list` | List all transactions |
| `sync-recovery cleanup` | Remove old transaction logs |

### Recovery Example

```
User: Sync failed, how do I recover?

AI: Checking transaction status...
    Last transaction: tx_20260319_152030
    Status: FAILED (interrupted at file 15/20)
    
    Would you like to:
    1. Rollback to pre-sync state
    2. Resume from file 15
    3. View transaction log
    
    [User selects 1]
    
    Rolling back transaction tx_20260319_152030...
    Restoring 14 files from backup...
    Rollback complete. Target directory restored to pre-sync state.
```

---

## Progress Feedback Mechanism

### Progress Report Format

```
┌─────────────────────────────────────────────────────────────┐
│                    Sync Progress                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Phase: File Synchronization                                │
│  Progress: ████████████░░░░░░░░ 60% (12/20 files)          │
│  Current: src/modules/feature_new.py                        │
│  Speed: ~150 files/second                                   │
│  ETA: ~5 seconds remaining                                  │
│                                                             │
│  Summary so far:                                            │
│  ✓ Copied: 8 files                                          │
│  ✓ Skipped: 4 files (unchanged)                             │
│  ⚠ Conflicts: 0                                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Progress Callback Configuration

```yaml
progress:
  # Enable progress reporting
  enabled: true
  
  # Update interval in milliseconds
  update_interval_ms: 500
  
  # Progress report format
  format: "detailed"  # "simple", "detailed", "json"
  
  # Callback for external systems
  callback:
    enabled: false
    url: ""
    method: "POST"
```

---

## Workflow Steps

### Step 1: Load Configuration

```
1. Determine config file path (user-specified > project root > default)
2. Read and parse YAML configuration
3. Validate required fields
4. Validate paths exist
5. Verify target is a git repository
6. Check git status (warn if uncommitted changes)
```

### Step 2: Directory Comparison

**Using Beyond Compare (if configured):**
```bash
BCompare.exe /silent /crc /fv="Newer" /ro "source_path" "target_path" /report="report.html"
```

**Using Built-in Method:**
```
1. Recursively scan source directory
2. For each file (excluding patterns):
   - Check if exists in target
   - Compare modification times
   - Detect conflicts (bidirectional changes)
   - Mark as: new, modified, conflict, unchanged, deleted
3. Generate comparison report
```

### Step 3: Process Manual Items

```
1. Check if any changed files match manual_items patterns
2. For each manual item:
   - Display the file and reason
   - Ask user for confirmation
   - If confirmed: include in sync
   - If rejected: skip this file
3. Continue with approved files only
```

### Step 4: Synchronize Files

```
1. If transactional_mode: Begin transaction
2. Create backup of target files (if enabled)
3. For each file to sync:
   - Check for conflicts
   - Apply conflict resolution strategy
   - Copy/overwrite file
   - Log operation
   - Report progress
4. Generate sync summary
5. If transactional_mode: Commit transaction
```

### Step 5: Git Operations

```
1. Check git status in target directory
2. If changes detected:
   a. Create new branch: brch_{auto_generated_name}
   b. Stage all changes: git add .
   c. Commit with generated message
   d. Push branch to remote
   e. If push_to_main is true:
      - Ask user confirmation
      - Switch to main branch
      - Merge the sync branch
      - Push to main
   f. Optionally create PR
3. If no changes:
   - Log "No changes to commit"
```

---

## Usage Examples

### Example 1: Automatic Sync After Tests

```
User: Tests passed, trigger sync workflow
Agent: [Invokes dir-sync-git skill]
       
       [1/7] Running pre-flight checks...
       ✓ Git installed
       ✓ Target is git repository
       ✓ Configuration valid
       ✓ Source directory exists
       ✓ Target directory exists
       ✓ Target working tree clean
       
       [2/7] Loading configuration... ✓
       [3/7] Comparing directories... ✓
              Found 5 modified files, 2 new files
              Detected 1 conflict: src/config.py (bidirectional)
       
       [4/7] Processing conflicts...
              Conflict: src/config.py
              Both source and target modified after last sync.
              Resolution strategy: manual
              
              Source modified: 2026-03-19 14:30:00
              Target modified: 2026-03-19 13:45:00
              
              Options:
              1. Use source (newer)
              2. Use target
              3. Skip this file
              4. View diff
              
              [User selects 1]
       
       [5/7] Processing manual items...
              config/production.yaml requires confirmation [Y/n]: Y
       
       [6/7] Synchronizing files (7 files)...
              ████████████████████ 100%
              ✓ Copied: 5 files
              ✓ Skipped: 2 files (unchanged)
              ✓ Resolved: 1 conflict
       
       [7/7] Git operations...
              Creating branch: brch_sync_20260319
              Committing changes...
              Push to main? [Y/n]: Y
              Merging to main...
              Pushing to remote...
       
       ✓ Sync complete!
       Branch: brch_sync_20260319
       Commit: abc1234
       Files: 7 synchronized
```

### Example 2: Manual Sync Request

```
User: Sync my dev directory to release
Agent: [Invokes dir-sync-git skill]
       
       Starting manual sync...
       Source: e:/program/trae/chart/code
       Target: e:/program/trae/chart/release
       
       [Executes full workflow with progress updates]
```

### Example 3: Dry Run

```
User: Show me what would be synced (dry run)
Agent: [Invokes dir-sync-git skill with dry-run mode]
       
       Dry run mode - no changes will be made
       
       Comparison results:
       ┌─────────────────────────────────────────────────────┐
       │ File                    │ Status    │ Action       │
       ├─────────────────────────────────────────────────────┤
       │ src/new_feature.py      │ NEW       │ Would copy   │
       │ src/main.py             │ MODIFIED  │ Would update │
       │ src/config.py           │ CONFLICT  │ Needs review │
       │ src/utils.py            │ UNCHANGED │ Skip         │
       │ .git/                   │ EXCLUDED  │ Skip         │
       │ *.log                   │ EXCLUDED  │ Skip         │
       └─────────────────────────────────────────────────────┘
       
       Summary:
       - Would sync: 2 files
       - Conflicts: 1 (requires resolution)
       - Excluded: 15 files
       
       No changes made (dry run).
```

---

## Error Handling

### Common Errors and Solutions

| Error | Cause | Solution |
|-------|-------|----------|
| Target not a git repo | Target path invalid | Run `git init` in target directory |
| Uncommitted changes | Target has pending changes | Commit or stash changes first |
| Beyond Compare not found | BC path incorrect | Use builtin method or fix path |
| Permission denied | File locked or no access | Close files, check permissions |
| Push rejected | Remote has new commits | Pull first, resolve conflicts |
| Transaction failed | Sync interrupted | Run `sync-recovery rollback` |
| Conflict detected | Bidirectional modification | Use conflict resolution strategy |

### Rollback Procedure

If sync causes issues:
```bash
# Option 1: Git revert (if already committed)
cd target_directory
git log --oneline -5  # Find the sync commit
git revert HEAD       # Revert last commit
git push origin main  # Push the revert

# Option 2: Transaction rollback (if transactional mode enabled)
sync-recovery rollback tx_20260319_152030
```

---

## Best Practices

1. **Always run dry-run first** - Preview changes before actual sync
2. **Review manual items carefully** - They are marked for a reason
3. **Keep exclusions updated** - Add generated files to exclusion list
4. **Enable transactional mode** - For critical syncs, enable rollback capability
5. **Test after sync** - Verify target still works
6. **Review commits** - Check the generated commit message
7. **Use conflict resolution** - Configure appropriate strategy for your workflow
8. **Monitor progress** - Watch for conflicts and errors during sync

---

## Version Compatibility

| Component | Minimum Version | Recommended |
|-----------|-----------------|-------------|
| Git | 2.0.0 | 2.30.0+ |
| Beyond Compare | 4.0 | 4.4+ |
| Python (for examples) | 3.6 | 3.10+ |

---

## Log Format Examples

### Sync Log

```
2026-03-19 15:20:30 - INFO - Starting sync transaction tx_20260319_152030
2026-03-19 15:20:30 - INFO - Source: e:/dev/project
2026-03-19 15:20:30 - INFO - Target: e:/release/project
2026-03-19 15:20:31 - INFO - BACKUP src/main.py → .sync-backup/main.py.1647700830.bak
2026-03-19 15:20:31 - INFO - SYNC src/main.py → target/src/main.py
2026-03-19 15:20:31 - INFO - CONFLICT src/config.py (bidirectional) → resolved: source_wins
2026-03-19 15:20:32 - INFO - COMMIT tx_20260319_152030
2026-03-19 15:20:32 - INFO - Sync complete: 5 files copied, 1 conflict resolved
```

### Conflict Log

```
2026-03-19 15:20:31 - WARNING - CONFLICT: src/config.py
  Type: bidirectional
  Source mtime: 2026-03-19 14:30:00
  Target mtime: 2026-03-19 13:45:00
  Last sync: 2026-03-19 12:00:00
  Resolution: source_wins
  Action: Copied source to target
```

---

## Integration Points

- **Pre-sync hook**: Run tests before sync
- **Post-sync hook**: Deploy or notify
- **Conflict resolution**: Manual merge interface
- **Audit log**: Track all sync operations
- **Recovery system**: Transaction rollback capability
