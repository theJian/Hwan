let execute = (args: list(string)) =>
  switch (args) {
    | ["init", ...args] => InitCommand.command.exec(args)
    | ["build", ...args] => BuildCommand.command.exec(args)
    | _ => HelpCommand.command.exec([])
  };
