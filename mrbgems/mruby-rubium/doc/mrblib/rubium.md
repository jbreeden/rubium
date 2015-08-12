# Module Rubium

## `attr_accessor :cache_path`
- Get or set the `cache_path` option.
- This tells rubium where to store cache data from the browser,
  such as localstorage and the like.
- If the specified path does not exists, rubium will create the directory.
- The parent of the indicated path must exist already.
- Default value: `nil` (No cache data will be saved)

## `attr_accessor :remote_debugging_port`
- Get or set the `remote_debugging_port` option (must be an int).
- This tells rubium what port (if any) to open for remote debugging
  via chrome dev tools.
- If specified, after launching the application you can navigate to
  `http://localhost:#{remote_debugging_port}` in a chrome window to
  access chrom dev tools for you app, inspect the html, and run javascript.
- Default value: `0` (disable remote debugging)

## `attr_accessor :server_port`
- Get or set the `server_port` option (must be an int)
- This tells rubium what port you're running your app server on.
- If you're app does not use a server, or you're server is not on localhost,
  this option has no effect.
- If you're running a server on localhost as part of your application, this
  option allows rubium to adjust the localstorage file names each time
  the app is launched so that previously set localstorage data can be accessed -
  even though the origin of the site (which includes the port) has actually changed.
- Default value: `nil`

## `attr_accessor :script_v8_extensions`
- Get or set the `script_v8_extensions` option (must be a string)
- This tells rubium what file to load when a new V8 context is created.
- In this file, you can use the [`mruby-cef`](https://github.com/jbreeden/mruby-cef/blob/master/doc/src/mruby_cef_v8.md)
  API to write javascript extension for use in your application. (See `javascript_interop` sample for examples).
- If this file does not exist, no error will be reported. It is simply ignored.
- Default value: `./rubium_v8_extensions.rb`

## `attr_accessor :url`
- Get or set the `url` option (must be a string)
- This tells rubium what url to load on app launch
- Accepts `file://`, `http://`, and `https://` urls
- Default value: `nil` (This is required however, you _must_ overwrite the default)

## `attr_accessor :use_page_titles`
- Get or set the `use_page_titles` option (boolean)
- This tells rubium whether to update the title bar with the page title specified in
  any loaded page.
- Default value: `false` (The `window_title` option will be used for the liftime of the app)

## `attr_accessor :window_title`
- Get or set the `window_title` option (must be a string)
- This tells rubium what text to display in the application's title bar
- Default value: `Rubium`

## `::on_launch(&block)`
- Provide a block to rubium specifying launch behavior.
- The provided block is only called when the app is started, and no existing
  instances are running.
- This is where you should specify the application options such as the url to load, etc.
  + Options are set by using the `attr_accessor`s described above.
  + Options set in the `on_launch` block are automatically loaded if another
    app instance is launched (see: `::on_relaunch`)

## `::on_relaunch(&block)`
- Provide a block to rubium specifying relaunch behavior.
- The provided block is only called when the app is started, but an instance
  is already running.
- The rubium options (such as `url`, `window_title`, etc.) set for the running
  application instance are re-used automatically

## `::open_new_window`
- Should only be called in the `on_relaunch` block.
- Opens a new window for the application.
- Same as running `window.open('/');` in the existing browser window.
