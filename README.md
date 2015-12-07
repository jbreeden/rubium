Rubium (alpha)
==============

A framework for developing desktop apps with web technologies.

&#x2713; Ruby <br/>
&#x2713; JavaScript <br/>
&#x2713; HTML <br/>
&#x2713; SVG <br/>
&#x2713; CSS <br/>

Rubium is similar in spirit to node-webkit, Brackets Shell, or GitHub's Electron.
It provides a Chromium shell for your application's UI, letting you develop your
UI like a web app, while also giving you access to the underlying system like a
native app.

Demo
----

Here is an example rubium app I call mruby-edit. It's a code editor written entirely in Ruby - including all DOM event handlers and manipulations. It's implemented on top of [Ace](https://ace.c9.io/#nav=about), but uses Ruby to configure
Ace keybindings, and access the file system. In the image, mruby-edit is viewing its own source code.

![demo](/screenshot.png)

Usage
-----

```
$ rubium --help
Rubium

Usage: rubium [OPTIONS...]

DESCRIPTION

  Desktop application development with Ruby, JavaScript, HTML, SVG, and CSS.

OPTIONS

  --cache-path=DIR
    The directory where rubium will persist cookies, local storage, etc.
    If this option is not supplied, no local storage will be saved.
    The directory, but no parents, will be created if it does not exist.

  --content-script=JS_FILE
    Path to a javascript file to be injected into each browser window.

  --dev-tools
    Open the Chromium dev tools in a separate window on launch.

  --help
    Show this help text.

  --url=URL
    On startup, open URL instead of searching for an index.html file.
    URL may be a local file, using the file:// protocol, or a remote resource
    addressed with http:// or https://

APPLICATION STARTUP

  The options for executing an application with Rubium are as follows:

  1) Specify the --url=URL parameter.
     Described above, under the "OPTIONS" heading.
  2) Run the `rubium` command in a folder with an index.html file.
     If this file is found, it is opened in a new window, the contents are
     displayed, and any scripts within are executed exactly as in a typical
     web application. This includes loading external scripts with <script>
     tags.

  3) Run the `rubium` command in a folder with no index.html file.
     In this case, the folder contents are displayed. An application may
     be launched by navigating to the file and selecting it.

RUNTIME ENVIRONMENT

  Multiple windows may be opened by the application. Each window will have it's
  own Ruby context as well as the usual JavaScript context. It is important to
  note that each Ruby context is indeed an independent VM instance, and so no
  variables may be shared among them. You may, however, use the browser's built
  in window messaging API to communicate among windows.

JAVASCRIPT & RUBY INTEROP

  To call Ruby from JavaScript, pass a string to the `ruby` function.

    Ex: Writing the working directory contents to the document
    ----------------------------------------------------------
    |  document.write(ruby('`ls`'));
    ----------------------------------------------------------

  Note that any value returned from Ruby to JavaScript must be converted to a
  JavaScript object. Rubium will do this automatically for built in types like
  Fixnums & booleans. If it does not know how to convert a value, undefined is
  returned instead. You can convert values manually using the JS.create_*
  family of functions.

  To interact with JavaScript objects - including the DOM - from Ruby,
  use the `JS` module. (Property access, method calls, and block arguments
  should all behave the way you expect.)

    Ex: Make an alert call on `window` explicitly from Ruby
    ----------------------------------------------------------
    |  JS.window.alert('Hell, from Ruby!')
    ----------------------------------------------------------

  As with pure JavaScript, any methods called on the JS module are delegated
  to the window object by default.

    Ex: Make an alert call on `window` implicitly from Ruby
    ----------------------------------------------------------
    |  JS.alert('Hell, from Ruby!')
    ----------------------------------------------------------

  Any block parameters to a JavaScript function are converted to function
  arguments automatically.

    Ex: Attaching a DOM event handler from Ruby
    ----------------------------------------------------------
    |  JS.document.addEventListener('onload') do |event|
    |    JS.alert('Document loaded!')
    |  end
    ----------------------------------------------------------

SEE ALSO:

  1) http://github.com/mruby/mruby
  The Ruby implementation embedded into Rubium.

  2) http://github.com/jbreeden/mruby-apr
  A re-implementation of much of the CRuby/MRI standard library for MRuby,
  used by Rubium to provide access to system resources like files & sockets.
```
