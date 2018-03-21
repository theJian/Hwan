/* Only define option we've used*/
type opt = Js.t({
  .
  cwd: string,
  absolute: bool
});

[@bs.module "glob"] external sync : (string, opt) => array(string) = "";
