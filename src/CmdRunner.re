type package = {
  version: option(string),
};

let decodePackageJson = (json) =>
  Json.Decode.{
    version: json |> optional(field("version", string))
  };

let readPackage = (path) => path |> Node.Fs.readFileAsUtf8Sync
                                     |> Json.parseOrRaise
                                     |> decodePackageJson;

let readRootPackage = () =>
  [|Node.Global.__dirname, "../../../", "package.json"|]
    |> Node.Path.join
    |> readPackage;

let getVersion = (package) =>
  switch(package.version) {
    | Some(version) => version
    | None => ""
  };

let echoVersion = () => readRootPackage() |> getVersion
                                          |> print_endline;

let runCommand = (command, package) => {
  print_endline("command: " ++ command);
  print_endline("package: " ++ package);
};

let run = ((command: string, options: CmdParser.options)) =>
  switch ((command, options)) {
  | (_, { version: true }) => echoVersion()
  | (command, { package }) => runCommand(command, package)
  };
