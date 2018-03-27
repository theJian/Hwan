let isProjectRoot = (path) => [|path, Config.packageJson|]
                                |> Node.Path.join
                                |> Node.Fs.existsSync;

let rec findRootDir = (cwd, ~root=?, ()) => {
  let root_ =
    switch (root) {
    | None => Node.Path.parse(cwd)##root
    | Some(r) => r
    };

  if (isProjectRoot(cwd)) cwd 
  else if (cwd != root_) findRootDir(Node.Path.dirname(cwd), ~root=root_, ())
  else {
    Logging.err("Could find the project root directory.");
    exit(1)
  }
};

let collectPackages = () => {
  let rootDir = findRootDir(Node.Process.process##cwd(), ());
  Glob.sync(Config.packagePaths, [%bs.obj { cwd: rootDir, absolute: true }])
    |> Array.to_list
    |> List.map(Package.concretizePackage)
    |> List.filter(Js.Option.isSome)
    |> List.map(Js.Option.getExn)
    |> Array.of_list
    ;
};
