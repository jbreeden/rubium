class MrubyEditor
  def initialize(aceEditor, commandLine = nil)
    @ace = aceEditor
    @document = @ace.getSession.doc
    @commandLine = commandLine
    bind_event_handlers
    if File.exists?("./MrubyEditorCommands.rb")
      self.instance_eval(File.read("./MrubyEditorCommands.rb"))
    end
  end

  # Syntax sugar for adding commands
  # Also creates a method on the MrubyEditor class
  # to invoke the command.
  def command(name, chord = nil, &block)
    # Add the command as a method
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
          self.instance_eval(@commandLine.textContent.to_s)
          if e.ctrlKey.bool_value
            @ace.focus
          end
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
