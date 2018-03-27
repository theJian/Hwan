type options = {
  version: bool,
  onlyDep: bool,
  package: string,
};

let parse = () => {
  let version = ref(false);
  let onlyDep = ref(false);
  let package = ref("");
  let anon = ref([]);
  let prepend = (a) => { anon := [a, ...anon^] };

  let speclist = [
    ("--version", Arg.Set(version), "Show version"),
    ("--package", Arg.Set_string(package), "Script execution target"),
    ("--only-dependencies", Arg.Set(onlyDep), "Only executing in dependencies"),
  ];
  let usagemsg = "Minimalistic monorepo management tool";

  Arg.parse(speclist, prepend, usagemsg);

  let command = anon^ |> List.rev |> List.tl;

  switch (command, version^) {
  | ([], false) => Arg.usage(speclist, usagemsg); exit(0)
  | _ => ignore
  };

  (
    command,
    {
      version: version^,
      package: package^,
      onlyDep: onlyDep^,
    }
  );
};
