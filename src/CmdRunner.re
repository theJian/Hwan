type nodeStatus = Nop | Include | Exclude;

let dirname: option(string) = [%bs.node __dirname];

let readRootPackage = () =>
  [|Js.Option.getExn(dirname), "../"|]
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

let queueTask = (graph, startNodeIds, ~exclude=[], ()) => {
  let nodeStatuses = Array.(make(length(graph), Nop));
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
  let cur = ref(
    Package.findLeftPNodeIds(graph) |> List.map(id => {
      switch (List.exists((==) @@ id, startNodeIds)) {
      | true => (id, Include)
      | false => (id, Exclude)
      }
    })
  );

  while (cur^ != []) {
    List.iter(((id, status)) => {
      nodeStatuses[id] = status;
      if (status == Include && !List.exists((==) @@ id, exclude)) {
        que := [id, ...que^];
      };
      ()
    }, cur^);

    cur := List.(
      cur^ |> map(((id, _)) => graph[id].depNodes)
              |> flatten
              |> sort_uniq(compare)
              |> map(id => {
                   let dependentStatuses =
                     dependents[id] |> map(id => nodeStatuses[id]);
                   let status = if (exists((==) @@ Nop, dependentStatuses)) {
                     Nop
                   } else if (exists((==) @@ Include, dependentStatuses)) {
                     Include
                   } else {
                     Exclude
                   };
                   (id, status)
                 })
              |> filter(
                   fun
                   | (_, Nop) => false
                   | _ => true
                 )
    )
  };
  que^ |> List.map(id => graph[id])
};

let syscall = (syscmd, node) => {
  let path = Package.(node.package.path);
  Node.Child_process.(
    execSync(
      syscmd,
      option(~cwd=path, ~encoding="utf8", ())
    )
  )
    |> Logging.output;
};

let runCommand = (command, options: CmdParser.options) => {
  Logging.logCommand(command);
  Logging.logPackage(options.package === "" ? "all" : options.package);

  let graph = CollectPackages.collectPackages() |> Package.buildPackageGraph;
  let startIds = findStartNodeIds(options.package, graph);
  queueTask(graph, startIds, ~exclude=options.onlyDep ? startIds : [], ())
    |> List.(iter(syscall(command |> fold_left((s1, s2) => s1 ++ " " ++ s2, ""))));
  ()
};

let run = ((command: list(string), options: CmdParser.options)) =>
  switch (command, options) {
  | (_, { version: true }) => echoVersion()
  | (command, options) => runCommand(command, options)
  };
