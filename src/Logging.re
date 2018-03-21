let err = msg => Printf.eprintf("[hwan] warning: %s\n%!", msg);

let output = msg => Printf.printf("[hwan] output:\n%s\n", msg);

let log = (type_, msg) => Printf.printf("[hwan] %s: %s\n", type_, msg);

let logCommand = command =>
  command
    |> List.fold_left((s1, s2) => s1 ++ " " ++ s2, "")
    |> log("Executing Command");

let logPackage = (package) =>
  switch (package) {
  | "" => "all"
  | package => package
  } |> log("Package");

