type packageMeta = {
  name: option(string),
  version: option(string),
  scripts: list((string, string)),
  deps: list((string, string)),
};

type package = {
  path: string,
  meta: packageMeta,
};

type pNode = {
  package,
  depNodes: list(int),
};

let hashtbl2pairlist = fun
  | Some(dict) => dict |> Js.Dict.entries |> Array.to_list
  | None => [];

let decodeDependencies = (json) =>
  Json.Decode.(
    [
      json |> optional(field("dependencies", dict(string))),
      json |> optional(field("devDependencies", dict(string))),
      json |> optional(field("peerDependencies", dict(string))),
      json |> optional(field("bundledDependencies", dict(string))),
      json |> optional(field("optionalDependencies", dict(string))),
    ]
      |> List.map(hashtbl2pairlist)
      |> List.flatten
    );

let decodePackageJson = (json) => {
  Json.Decode.{
    name: json |> optional(field("name", string)),
    version: json |> optional(field("version", string)),
    scripts: json |> optional(field("scripts", dict(string))) |> hashtbl2pairlist,
    deps: json |> decodeDependencies,
  }
};

let readPackageMeta = (path) => [|path, Config.packageJson|]
                               |> Node.Path.join
                               |> Node.Fs.readFileAsUtf8Sync
                               |> Json.parseOrRaise
                               |> decodePackageJson;

let concretizePackage = (path): option(package) =>
  switch ([|path, Config.packageJson|] |> Node.Path.join |> Node_fs.existsSync) {
  | true => Some({ path, meta: path |> readPackageMeta })
  | false => None
  };

let buildPNode = (pkgIdx, package) => {
  package,
  depNodes: package.meta.deps
    |> List.split
    |> fst
    |> List.fold_left((localDeps, dep) => {
      switch (Hashtbl.find(pkgIdx, dep)) {
        | exception Not_found => localDeps
        | idx => [idx, ...localDeps]
        }
    }, [])
};

let buildPackageGraph = (packages) => {
  let size = Array.length(packages);
  let pkgIdx = Hashtbl.create(size);
  packages |> Array.iteri((idx, pkg) =>
    switch (pkg.meta.name) {
    | Some(name) => Hashtbl.add(pkgIdx, name, idx)
    | None => ()
    }
  );

  packages |> Array.map(buildPNode(pkgIdx))
};


let findRootPNodes = graph => graph |> Array.to_list
                                    |> List.filter(fun
                                         | [] => true
                                         | _ => false
                                       );

let findLeftPNodeIds = graph => {
  let isLeft = Array.(make(length(graph), true));
  graph |> Array.map(node => node.depNodes)
        |> Array.to_list
        |> List.flatten
        |> List.fold_left((a, idx) => { a[idx] = false; a }, isLeft)
        |> Array.to_list
        |> List.mapi((idx, isLeft) => isLeft ? idx : -1)
        |> List.filter(id => id != -1)
};

let findNodeId = (f, graph) =>
  graph
    |> Array.mapi((idx, node) => f(node) ? idx : -1)
    |> Array.to_list
    |> List.filter((!=)(-1))
    |> List.hd
