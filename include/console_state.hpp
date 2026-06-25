#pragma once

#include <string>

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

  const char *current_state() const override;
};

class MainMenuState : public ConsoleBaseState {
public:
  void handle(Console &console, const ParsedCommand &command) override;
  bool accepts(const ParsedCommand &command) const override;
  const char *commandPrompt() const override;
  const char *invalidCommandMessage() const override;
  const char *current_state() const override;
};

class ProcessScreenState : public ConsoleBaseState {
public:
  ProcessScreenState(const std::string& processName);

  void handle(Console &console, const ParsedCommand &command) override;
  bool accepts(const ParsedCommand &command) const override;
  const char *commandPrompt() const override;
  const char *invalidCommandMessage() const override;
  const char *current_state() const override;

private:
  std::string attachedProcessName; 
  std::string customPrompt;
  std::string customState;
};