type options = {
  version: bool,
  package: string,
};

type anon = list(string);

let parseScript =
  fun
  | [_, "run", script, ..._] => script
  | [_, script, ..._] => script
  | _ => "no such a fucking command";

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
  let usageMsg = "Minimalistic monorepo management tool";

  Arg.parse(speclist, prepend, usageMsg);

  (
    parseScript(List.rev(anon^)),
    {
      version: version^,
      package: package^,
    }
  );
};
