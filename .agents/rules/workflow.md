# Agent Workflow Rules

## Purpose

Rules for AI agents (like OpenCode) working on this repository alongside a human developer.

## Core Principles

1. **Never merge** - Always wait for human approval before merging PRs
2. **Ask for clarification** - When requirements are ambiguous, ask before guessing
3. **Make focused changes** - One logical change per PR
4. **Test before reporting** - Verify CI passes before reporting success

## Before Creating a PR

### Checklist

- [ ] All files changed are in the PR
- [ ] CMakeLists.txt updated if adding new source files
- [ ] Tests added/updated if adding functionality
- [ ] CI passes locally (if possible)
- [ ] Commit message is clear and follows conventions
- [ ] CHANGELOG.md updated with entry under `[Unreleased]`
- [ ] PROGRESS.md updated if phase status changed

### Branch Naming

- `feature/*` - New features
- `fix/*` - Bug fixes
- `refactor/*` - Code refactoring (no behavior change)
- `docs/*` - Documentation only

## After CI Passes

1. Report status to human
2. List what was changed
3. Wait for human approval
4. **Do NOT merge** - Even if CI passes

## Communication Format

When reporting PR status, use:

```
## PR #[number]: [title]

**Status:** CI passed | CI failed

**Changes:**
- [list of changes]

**Next:** Awaiting your approval to merge
```

## What Agents Should NEVER Do

- ❌ Merge PRs
- ❌ Force push to main or protected branches
- ❌ Delete branches (unless explicitly asked)
- ❌ Access secrets or credentials
- ❌ Modify `.github/workflows/` without explicit permission
- ❌ Bypass CI requirements
- ❌ Create release tags

## What Agents Should ALWAYS Do

- ✅ Create PRs for all changes
- ✅ Wait for human approval before merge
- ✅ Report CI failures with error details
- ✅ Update CHANGELOG.md when changes are made (add entry under `[Unreleased]`)
- ✅ Update PROGRESS.md when phases are completed or status changes
- ✅ Update AGENTS.md or docs when processes change
- ✅ Ask for clarification on ambiguous requests
