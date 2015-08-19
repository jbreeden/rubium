JS.window[:MrubyEditor] = JS.function do |aceEditor, commandLine|
  unless aceEditor && commandLine
    raise "MrubyEditor expects two arguments - an ace editor, and a div with editable content for the command line"
  end
  $ed = MrubyEditor.new(aceEditor, commandLine)
end

class MrubyEditor
  def initialize(aceEditor, commandLine)
    @ace = aceEditor
    @session = @ace.getSession
    @document = @session.doc
    @commandLine = commandLine

    @ace.setOptions({
      mode: "ace/mode/ruby",
      tabSize: 2,
      useSoftTabs: true
    });

    bind_event_handlers
    if File.exists?("./MrubyEditorCommands.rb")
      self.instance_eval(File.read("./MrubyEditorCommands.rb"))
    end
    @user_state = {}

    use_editor_font_in_command_line
    restore_editor_state

    @ace.focus
  end

  def use_editor_font_in_command_line
    editor_style = JS.getComputedStyle(@ace.container)
    @commandLine.style.fontFamily = editor_style.fontFamily;
    @commandLine.style.fontSize = editor_style.fontSize
  end

  def save_editor_state
    JS.localStorage.script = @ace.getValue
    JS.localStorage.commandLine = @commandLine.textContent
  end

  def restore_editor_state
    unless JS.localStorage.script.is_undefined?
      @ace.setValue JS.localStorage.script
    end

    unless JS.localStorage.commandLine.is_undefined?
      @commandLine.textContent = JS.localStorage.commandLine
    end
  end

  def save_user_state
    @user_state[:scroll_top] = @session.getScrollTop
  end

  def save_state
    save_editor_state
    save_user_state
  end

  def restore_user_state
    @session.setScrollTop @user_state[:scroll_top] if @user_state[:scroll_top]
  end

  def maintain_user_state(&block)
    save_state # editor & user states (so we can restore editor in case of crash)
    block[]
    restore_user_state
  end

  # Syntax sugar for adding commands
  # Also creates a method on the MrubyEditor class
  # to invoke the command.
  def command(name, chord = nil, &block)
    # Add the command as a method for quick command line use.
    # Recall the command line text is eval'ed in the context of the editor.
    self.singleton_class.instance_eval do
      define_method name do
        block[]
      end
    end

    # Add the command to the aceEditor (mostly for keybindings)
    commands.addCommand({
        name: name,
        bindKey: chord,
        exec: JS.function(&block)
    })
  end

  def bind_event_handlers
    if @commandLine
      command 'gotoCommandLine', 'Ctrl-Tab' do
        @commandLine.focus
      end

      @commandLine.addEventListener 'keydown' do |e|
        if e.ctrlKey.bool_value && e.keyCode.int_value == "\t".ord # Tab
          @ace.focus
        end
      end

      @commandLine.addEventListener 'keydown' do |e|
        if e.keyCode.int_value == 13 # Enter
          e.preventDefault
          e.stopPropagation
          save_editor_state
          self.instance_eval(@commandLine.textContent.to_s)
          if e.ctrlKey.bool_value
            @ace.focus
          end
        end
      end
    end

    command "runSelections", "Ctrl-R" do
      maintain_user_state do
        self.eval
      end
    end

    command "runSelectionsAndPrint", "Ctrl-Shift-R" do
      maintain_user_state do
        results = self.eval
        lines = []
        @document.getAllLines.forEach do |line|
          lines.push(line.to_s)
        end
        adjustment = 1
        results.keys.each do |row_num|
          result = results[row_num]
          next unless result
          result = result.to_s.gsub(/\r\n|\r/, "\n")
          lines = result.split("\n")
          @document.insertFullLines(row_num + adjustment, lines)
          adjustment += lines.length
        end
      end
    end
  end

  def multiple_cursors?
    @ace.selection.ranges.length.int_value != 0
  end

  def selection?
    each_range do |range|
      return true unless range.isEmpty.bool_value
    end
    return false
  end

  def get_text_range(range)
    @document.getTextRange(range).to_s
  end

  def each_cursor(&block)
    @ace.forEachSelection({ exec: JS.function(&block) })
  end

  def each_range(&block)
    if multiple_cursors?
      @ace.selection.getAllRanges.forEach(&block)
    else
      cursor = @ace.selection.getCursor
      range = @ace.selection.getRange
      # With getAllRanges, each range has a cursor reference.
      # This normalizes the single range case so that all calls are the same.
      range[:cursor] = cursor
      block[range]
    end
  end

  def eval
    results = {}

    each_cursor do
      if @ace.selection.getRange.isEmpty.bool_value
        @ace.navigateLineStart
        @ace.selection.selectLineEnd
      end
    end

    each_range do |range|
      begin
        results[range.end.row.int_value] = Kernel.eval(get_text_range(range)).to_s
      rescue Exception => ex
        err = ex.inspect.to_s
        JS.console.warn(err)
        results[range.end.row.int_value] = err
      end
    end

    results
  end

  def method_missing(name, *args, &block)
    @ace.send(name, *args, &block)
  end
end
