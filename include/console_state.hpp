#pragma once

class Console;
struct ParsedCommand;

class ConsoleBaseState {
public:
  virtual ~ConsoleBaseState() = default;

  virtual void handle(Console &console, const ParsedCommand &command) = 0;
  virtual bool accepts(const ParsedCommand &command) const = 0;
  virtual const char *commandPrompt() const = 0;
  virtual const char *invalidCommandMessage() const = 0;
  virtual const char *current_state() const = 0;
};

class UninitializedState : public ConsoleBaseState {
public:
  void handle(Console &console, const ParsedCommand &command) override;
  bool accepts(const ParsedCommand &command) const override;
  const char *commandPrompt() const override;
  const char *invalidCommandMessage() const override;

  const char *current_state() const override { return "Cli uninitialized"; }
};

class MainMenuState : public ConsoleBaseState {
public:
  void handle(Console &console, const ParsedCommand &command) override;
  bool accepts(const ParsedCommand &command) const override;
  const char *commandPrompt() const override;
  const char *invalidCommandMessage() const override;

  const char *current_state() const override { return "Cli main menu"; }
};

class ProcessScreenState : public ConsoleBaseState {
public:
  void handle(Console &console, const ParsedCommand &command) override;
  bool accepts(const ParsedCommand &command) const override;
  const char *commandPrompt() const override;
  const char *invalidCommandMessage() const override;

  const char *current_state() const override { return "Cli process screen"; }
};
