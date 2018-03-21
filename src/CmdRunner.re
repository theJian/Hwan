let readRootPackage = () =>
  [|Node.Global.__dirname, "../../../"|]
    |> Node.Path.join
    |> Package.readPackageMeta;

let getVersion = (package) =>
  Package.(
    switch(package.version) {
    | Some(version) => version
    | None => ""
    }
  );

let echoVersion = () => readRootPackage() |> getVersion
                                          |> print_endline;

let findStartNodeIds = (name, graph) => {
  Package.(
    switch (name, graph) {
      | ("", graph) => findLeftPNodeIds(graph)
      | (name, graph) => graph
        |> findNodeId(node => switch (node.package.meta.name) {
          | Some(pkgName) => pkgName == name
          | None => false
          })
        |> a => [a]
    }
  )
};

let queueTask = (graph, startNodeIds) => {
  let visited = Array.(make(length(graph), false));
  let dependents =
    graph
      |> Array.to_list
      |> List.mapi((id1, node: Package.pNode) => node.depNodes |> List.map(id2 => (id2, id1)))
      |> List.flatten
      |> List.fold_left((deps, (id1, id2)) => {
        deps[id1] = [id2, ...deps[id1]];
        deps
      }, Array.(make(length(graph), [])));
  let que = ref([]);
  let curIds = ref(startNodeIds);
  while (curIds^ != []) {
    List.iter(id => {
      visited[id] = true;
      que := [id, ...que^];
      ()
    }, curIds^);

    curIds := List.(
      curIds^ |> map(id => graph[id].depNodes)
              |> flatten
              |> sort_uniq(compare)
              |> filter(id => {
                   for_all(id_ => visited[id_], dependents[id])
                 })
    )
  };
  que^ |> List.map(id => graph[id])
};

let syscall = (syscmd, node) => {
  let path = Package.(node.package.path);
  Node.(
    ChildProcess.execSync(
      syscmd,
      Options.options(~cwd=path, ~encoding="utf8", ())
    )
  )
    |> Logging.output;
};

let runCommand = (command, package) => {
  Logging.logCommand(command);
  Logging.logPackage(package === "" ? "all" : package);

  let graph = CollectPackages.collectPackages() |> Package.buildPackageGraph;
  let startIds = findStartNodeIds(package, graph);
  queueTask(graph, startIds)
    |> List.(iter(syscall(command |> fold_left((s1, s2) => s1 ++ " " ++ s2, ""))));
  ()
};

let run = ((command: list(string), options: CmdParser.options)) =>
  switch (command, options) {
  | (_, { version: true }) => echoVersion()
  | (command, { package }) => runCommand(command, package)
  };
