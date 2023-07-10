// import Dashboard from "./Dashboard";
// export default Dashboard;

/*

export { default } from "./myModule"
export { default as mySecondModule } from "./mySecondModule"
export { myNamedModule } from "./myNamedModule"

For default exports, all we need to do is to export the default.
For named exports from default modules all we need to do is export them as a name that we give them via default as mySecondModule.
Lastly, for named exports from named imports we just need to replace the import keyword with export.

another way if module is a named module and you wanna export it as default:
export { Dashboard as default } from './Dashboard';

*/

export { default } from './Dashboard';

