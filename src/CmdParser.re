type options = {
  version: bool,
  package: string,
};

type anon = list(string);

type script = option(string);

let parseScript =
  fun
  | [_, "run", script, ..._] => Some(script)
  | [_, script, ..._] => Some(script)
  | _ => None;

let parse = () => {
  let version = ref(false);
  let package = ref("");
  let anon = ref([]);
  let prepend = (a) => { anon := [a, ...anon^] };

  let speclist = [
    ("-v", Arg.Set(version), "Show version"),
    ("--version", Arg.Set(version), "Show version"),
    ("--hwan-pkg", Arg.Set_string(package), "Script execution target"),
  ];
  let usagemsg = "Minimalistic monorepo management tool";

  Arg.parse(speclist, prepend, usagemsg);

  let script =
    switch (parseScript(List.rev(anon^))) {
    | Some(script) => script
    | None => Arg.usage(speclist, usagemsg); exit(0)
    };

  (
    script,
    {
      version: version^,
      package: package^,
    }
  );
};
