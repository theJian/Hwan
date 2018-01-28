type executionResult = Ok | Error(string);

type opt = {
  option: string,
}

type command = {
  command: string,
  summary: string,
  example: option(string),
  options: list(opt),
  exec: list(string) => executionResult
};

let build = (~command, ~describe, ~example=?): command => {
  command,
  describe,
  example,
  exec: (args) => {
    print_endline("command: " ++ command);
    print_endline("describe: " ++ describe);

    switch (example) {
      | Some(message) => {
          print_endline("example: ");
          print_endline(message);
        }
      | None => ()
    };

    Ok
  },
};

let opt = ()
