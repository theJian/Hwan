let command = Command.build(
  ~command="init",
  ~summary="Create a new hwan repository.",
  ~example={|
    Create a new hwan repository for an existing codebase
    $ cd /path/to/my/codebase
    $ hwan init
  |}
);
