import { Application, Router } from "https://deno.land/x/oak/mod.ts";

const app = new Application();

const CONTENT_ROOT = Deno.args[1];

console.log(Deno.args);
console.log(`Content Root: ${CONTENT_ROOT}`);

app.use(async (context) => {
  await context.send({
    root: CONTENT_ROOT,
  });
});

app.addEventListener("listen", () => {
  console.log("Server started");
});

await app.listen({ port: 8000 });
