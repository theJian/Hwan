let getCommandLineArgs = (args) => switch (args) {
  | [_, _, ...commandLineArgs] => commandLineArgs
  | _ => []
};

let () =
  Sys.argv
  |> Array.to_list
  |> getCommandLineArgs
  |> Cli.execute
  |> ignore
  ;
