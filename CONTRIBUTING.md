# Contributing to RF4s-ScriptLoader

*Hey, modifier of fate—welcome to the Queer Codex.*

Whether you're rewriting romance logic, forging inclusive pronoun support, or just cleaning up console output, your code belongs here.
This project exists because some of us grew up loving games that didn’t love us back. So we’re making them better—one hook, patch, and dialogue branch at a time.

Be kind to your future self and others: comment weird decisions, test before committing chaos, and leave the code a little gayer than you found it.

~ The Gay Agenda (Technical Branch)

## The Basics

- **Found a bug?** Open an issue with a clear description and steps to reproduce
- **Fixed a bug?** Submit a pull request referencing the issue
- **Adding a feature?** Open an issue to discuss it first before coding
- **Have questions?** Use GitHub discussions rather than issues

## Getting Started

1. Fork the repository
2. Clone your fork:
    
    ``` bash
    git clone https://github.com/username/rf4s-scriptloader.git
    ```
    
3. Create a branch:
    
    ``` bash
    git checkout -b fix-awesome-bug
    ```
    
4. Make your changes
5. Commit using [Conventional Commits](https://www.conventionalcommits.org/).
6. Push to your fork:
    
    ``` bash
    git push origin fix-awesome-bug
    ```
    
7. Submit a pull request!

## Development Process

- Follow our [Git Workflow](https://github.com/8f00ff/knowledge-base/blob/main/Git/Git%20Workflow.md) guidelines
- Write tests for your changes
- Update documentation if needed
- Be kind and respectful in all interactions

## Code Style

We strive for clean, maintainable, and well-documented code out here.
This isn’t corporate C++, but it’s not spaghetti either.

### Guidelines

* Use **2-space indentation**, no tabs
Braces on the same line (if (...) {, not on their own line) — aka K&R / 1TBS style
* Stick to **C-style C++**, minimal abstractions — keep it readable, even in disassembly
* Prefer **explicit control flow** over clever tricks
* Leave **comments** when hooking or patching low-level logic
* If it feels like a hack, it probably is — document it

When in doubt: readable > clever.
Write it so future-you (or someone else) can understand what you were thinking at 3am.

## Project Structure

- `src/` - Source code for the library
- `Makefile` - Build configuration filefor make
- `flake.nix` - Nix shell environment

## Attribution Requirements

This project follows specific attribution guidelines that MUST be maintained in all derivatives and forks.

### [ATTRIBUTIONS.md](ATTRIBUTIONS.md) Structure

#### Maintainers

Current active maintainers only. Original authors should NOT be listed here unless actively maintaining this fork. Temporary PR forks (intended to be merged & deleted) don't need to modify this section, but long-term or permanent forks MUST update it to reflect actual maintainers.

#### Original Authors

Original creators of the project. This section MUST remain intact and unmodified in ALL derivatives and forks.

#### Additional Contributors

All original contributors MUST be kept, new ones may be appended. PR authors should add themselves here.

#### Supporters

All original supporters MUST be preserved, new ones may be appended.

#### Third-Party Assets & Libraries

All third-party assets, libraries, and tools used by the project. Each entry MUST include:
- Name with link to source
- Brief description of the asset/tool
- License information in parentheses

All entries MUST remain in this section as long as the corresponding asset or library is being used in any part of the project. New dependencies should be appended as they are added. When a dependency is removed, its entry may be removed from this section, as git history preserves the attribution record.

### Donation Links Policy

- Original creator donation/support links, if any exist, MUST always be maintained in all derivatives.
- Derivative works may include their own donation/support links.
- Derivative creator links may be positioned above original creator links, but original links must remain present and functional.

### Fork Responsibility

- Original authors reserve the right to request correction of attribution in any fork that misrepresents maintainer status.
- "PR-only" forks that become long-term projects MUST update the Maintainers section to accurately reflect who is actively maintaining the fork.

**Using this project constitutes agreement to follow these attribution guidelines.**

## Questions?

Feel free to open a discussion if you have any questions!
