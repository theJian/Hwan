let parse = (args: list(string)) =>
  switch (args) {
    | ["init", ...args] => InitCommand.command.exec(args)
    | ["run", ...args] => RunCommand.command.exec(args)
    | _ => HelpCommand.command.exec([])
  }
