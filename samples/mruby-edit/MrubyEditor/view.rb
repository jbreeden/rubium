class MrubyEditor::View
  attr_accessor :container
  attr_accessor :editor
  attr_accessor :session
  attr_accessor :document
  attr_accessor :commandLinePanel
  attr_accessor :commandLineOutput
  attr_accessor :commandLine
  attr_accessor :commandLineResizer
  attr_accessor :fileName
  attr_accessor :cwd
  attr_accessor :commandPrompt
  attr_accessor :minimizeOutputButton
  attr_accessor :maximizeOutputButton

  def initialize(model, selector)
    @model = model
    init_dom_elements(selector)
    init_dom_listeners
    init_model_listeners
  end

  def init_dom_elements(selector)
    @container = JS.document.querySelector(selector)
    @container.innerHTML = HTML
    @editor = JS.ace.edit(@container.querySelector('.mruby-edit-panel'))
    @session = @editor.getSession
    @document = @session.doc
    @commandLinePanel = @container.querySelector('.mruby-command-line-panel')
    @commandLineOutput = @container.querySelector('.mruby-command-line-output')
    @commandLine = @container.querySelector('.mruby-command-line')
    @commandLineResizer = @container.querySelector('.mruby-command-line-resizer')
    @fileName = @container.querySelector('.mruby-file-name')
    @cwd = @container.querySelector('.mruby-cwd')
    @commandPrompt = @container.querySelector('.mruby-command-prompt')
    @clearOutputButton = @container.querySelector('.clear-output')
    @minimizeOutputButton = @container.querySelector('.minimize-output')
    @maximizeOutputButton = @container.querySelector('.maximize-output')
    style = JS.document.createElement('style')

    style.textContent = CSS_FORMAT % { selector: selector.to_s }

    JS.document.head.appendChild(style)
    # Temporary (probably)
    JS.window.editor = @editor

    @editor.setOptions({
      mode: "ace/mode/ruby",
      tabSize: 2,
      useSoftTabs: true
    })

    use_editor_font_in_command_line

    # Don't show by default
    @editor.setPrintMarginColumn(-1)
  end

  def use_editor_font_in_command_line
    editor_style = JS.getComputedStyle(@editor.container)
    @commandLinePanel.style.fontFamily = editor_style.fontFamily;
    @commandLinePanel.style.fontSize = editor_style.fontSize
  end

  def init_dom_listeners
    @editor.addEventListener 'mouseup' do
      @model.focused_panel.set :editor
    end

    @commandLinePanel.addEventListener 'keydown' do |e|
      if e.ctrlKey.bool_value && e.keyCode.int_value == "\t".ord # Tab
        @model.rotate_panel_focus
      end
    end

    @commandPrompt.addEventListener 'click' do
      @model.rotate_terminal_mode
      @commandLine.focus
    end

    @commandLine.addEventListener 'keydown' do |e|
      if e.keyCode.int_value == 13 # Enter
        e.preventDefault
        e.stopPropagation
        @model.command_line.set @commandLine.textContent.to_s
        @model.command_invoked.trigger
        if e.ctrlKey.bool_value
          @model.rotate_panel_focus
        end
      end
    end

    @model.command_completed.subscribe do
      oldOutput = @model.command_line_output.get
      oldOutput = oldOutput.split("\n").last(500).join("\n")
      @model.command_line_output.set(oldOutput + "#{"\n" if oldOutput.length > 0 }#{@model.command_prompt.get} #{@model.command_line.get}\n#{@model.last_command_result.get}")
      @commandLineOutput.scrollTop = JS.Infinity
    end

    @commandLineResizer.addEventListener('mousedown') do |e|
      drag_start_y = e.screenY.int_value
      drage_start_output_height = if @commandLineOutput.style.display == 'none'
        0
      else
        @commandLineOutput.clientHeight.int_value
      end
      e.stopPropagation
      e.preventDefault

      on_drag = JS.function do |e|
        new_height = (drag_start_y - e.screenY.int_value + drage_start_output_height)
        if new_height <= 0
          hide_command_output
        else
          show_command_output
          resize_command_output(new_height)
        end
        e.stopPropagation
        e.preventDefault
      end

      on_drag_end = JS.function do |e|
        JS.document.removeEventListener('mousemove', on_drag)
        JS.document.removeEventListener('mouseup', on_drag_end)
      end

      JS.document.addEventListener('mousemove', on_drag)
      JS.document.addEventListener('mouseup', on_drag_end)
    end

    @clearOutputButton.addEventListener('click') do
      @model.command_line_output.set ''
    end

    @minimizeOutputButton.addEventListener('click') do
      hide_command_output
    end

    @maximizeOutputButton.addEventListener('click') do
      show_command_output
    end
  end

  def init_model_listeners
    @model.file_name.subscribe(:immediate) do |new_value|
      @fileName.textContent = new_value
    end

    @model.cwd.subscribe(:immediate) do |new_value|
      @cwd.textContent = "[#{new_value}]"
    end

    @model.command_prompt.subscribe(:immediate) do |new_value|
      @commandPrompt.textContent = new_value
    end

    @model.focused_panel.subscribe do |panel|
      case panel
      when :editor
        @editor.focus
      when :command_line
        @commandLine.focus
      end
    end

    @model.restoring_editor_state.subscribe do |is_restoring|
      unless is_restoring
        @commandLine.textContent = @model.command_line.get
        @editor.setValue @model.script.get
      end
    end

    @model.command_line_output.subscribe do |new_value|
      @commandLineOutput.textContent = new_value
    end
  end

  def show_command_output
    @commandLineOutput.style.display = "block"
    @editor.resize
  end

  def hide_command_output
    @commandLineOutput.style.display = "none"
    @editor.resize
  end

  def resize_command_output(new_height)
    @commandLineOutput.style.height = new_height.to_s + "px"
    @editor.resize
  end

  CSS_FORMAT = <<EOS
  %{selector} .mruby-editor-wrapper {
    align-items: stretch;
    display: flex;
    flex-direction: column;
    height: 100%%;
  }

  %{selector} .mruby-edit-panel {
    flex-grow: 1;
    width: 100%%;
  }

  %{selector} .mruby-command-line-panel {
    background-color: rgba(0, 0, 0, 0.1);
    border-color: rgba(0, 0, 0, 0.2);
    border-style: solid;
    border-width: 1px;
    padding: 0 4px 4px 4px;
  }

  %{selector} .mruby-command-line-resizer {
    cursor: ns-resize;
    display: flex;
    flex-direction: row;
    flex-shrink: 0;
    align-items: center;
    margin: 0;
    text-align: right;
  }

  %{selector} .mruby-command-line-resizer img {
    height: 10px;
    margin: 0;
    padding: 0;
    padding: 4px;
    width: 10px;
  }

  %{selector} .mruby-file-name {
    color: grey;
  }

  %{selector} .button {
    border-color: rgba(0, 0, 0, 0);
    border-radius: 4px;
    border-style: solid;
    border-width: 1px;
    cursor: initial;
    user-select: none;
    padding: 1px;
    font-family: inherit;
    font-size: inherit;
    background-color: transparent;
  }

  %{selector} .button:focus {
    outline-style: none;
  }

  %{selector} .button:hover {
    border-color: rgba(0, 0, 0, 0.2);
    cursor: initial;
  }

  %{selector} .button:active {
    background-color: rgba(0, 0, 0, 0.2);
    outline-style: none;
  }

  %{selector} .mruby-command-line-output {
    flex-grow: 1;
    font-family: inherit;
    font-size: inherit;
    height: 100px;
    margin: 0 0 2px 0;
    overflow: scroll;
  }

  %{selector} .mruby-command-prompt-panel {
    align-items: center;
    display: flex;
    flex-director: row;
    flex-grow: 0;
    flex-shrink: 0;
    padding-top: 2px;
  }

  %{selector} .mruby-cwd {
    padding-top: 2px;
  }

  %{selector} .mruby-command-line-output {
    display: none;
    color: whitesmoke;
    background-color: black;
  }

  %{selector} .mruby-cwd {
    border-color: rgba(0, 0, 0, 0.2);
    border-top-style: solid;
    border-width: 1px;
    color: grey;
  }

  %{selector} .mruby-command-line {
    display: inline-block;
    flex-grow: 1;
    margin-left: 5px;
  }

  %{selector} .mruby-command-line:focus {
    outline-style: none;
  }

  %{selector} .spacer {
    flex-grow: 1;
  }

  %{selector} .noflex {
    flex-grow: 0;
    flex-shrink: 0;
  }
EOS

  HTML = <<EOS
<div class="mruby-editor-wrapper">
  <div class="mruby-edit-panel"></div>
  <div class="mruby-command-line-panel">
    <div class="mruby-command-line-resizer">
      <div class="mruby-file-name"></div>
      <div class="spacer"></div>
      <img class="button clear-output noflex" src="icons/svg/cross89.svg"></img>
      <img class="button minimize-output noflex" src="icons/svg/down80.svg"></img>
      <img class="button maximize-output noflex" src="icons/svg/up131.svg"></img>
    </div>
    <pre class="mruby-command-line-output"></pre>
    <div class="mruby-cwd"></div>
    <div class="mruby-command-prompt-panel">
      <button class="button mruby-command-prompt"></button>
      <div contentEditable="true" class="mruby-command-line"></div>
    </div>
  </div>
</div>
EOS
end
