class MrubyEditor
  def initialize(aceEditor, commandLine = nil)
    @aceEditor = aceEditor
    @document = @aceEditor.getSession.doc
    @commandLine = commandLine
    register_commands
  end

  # Syntax sugar, used in register_commands
  def command(name, chord, &block)

    commands.addCommand({
        name: name,
        bindKey: chord,
        exec: JS.function(&block)
    })
  end

  # Handles mutliple cursors
  def cursor_command(name, chord, &block)
    commands.addCommand({
        name: name,
        bindKey: chord,
        exec: JS.function do
          if @aceEditor.selection.rangeCount.int_value > 1
            @aceEditor.forEachSelection({ exec: JS.function(&block) })
          else
            block[]
          end
        end
    })
  end

  def register_commands
    cursor_command 'navigateUp', 'Ctrl-P' do
      @aceEditor.navigateUp(1)
    end

    cursor_command 'selectUp', 'Ctrl-Shift-P' do
      @aceEditor.selection.selectUp(1)
    end

    cursor_command 'navigateDown', 'Ctrl-N' do
      @aceEditor.navigateDown(1)
    end

    cursor_command 'selectDown', 'Ctrl-Shift-N' do
      @aceEditor.selection.selectDown(1)
    end

    cursor_command 'navigateLeft', 'Ctrl-B' do
      @aceEditor.navigateLeft(1)
    end

    cursor_command 'selectLeft', 'Ctrl-Shift-B' do
      @aceEditor.selection.selectLeft(1)
    end

    cursor_command 'navigateRight', 'Ctrl-F' do
      @aceEditor.navigateRight(1)
    end

    cursor_command 'selectRight', 'Ctrl-Shift-F' do
      @aceEditor.selection.selectRight(1)
    end

    cursor_command 'navigateLineStart', 'Ctrl-A' do
      @aceEditor.navigateLineStart
    end

    cursor_command 'selectLineStart', 'Ctrl-Shift-A' do
      @aceEditor.selection.selectLineStart
    end

    cursor_command 'navigateLineEnd', 'Ctrl-E' do
      @aceEditor.navigateLineEnd
    end

    cursor_command 'selectLineEnd', 'Ctrl-Shift-E' do
      @aceEditor.selection.selectLineEnd
    end

    cursor_command 'splitIntoLines', 'Ctrl-Shift-L' do
      @aceEditor.selection.splitIntoLines
    end

    command 'undo', 'Ctrl-Z' do
      @aceEditor.undo
    end

    command 'redo', 'Ctrl-Shift-Z' do
      @aceEditor.redo
    end

    if @commandLine
      @aceEditor.container.addEventListener "keyup" do |e|
        if e.keyCode.int_value == 27 # Escape
          e.stopPropagation
          e.preventDefault
          @aceEditor.blur
          @commandLine.focus
        end
      end

      @commandLine.addEventListener 'keyup' do |e|
        if e.keyCode.int_value == 27 # Escape
          @commandLine.blur
          @aceEditor.focus
        end
      end

      @commandLine.addEventListener 'keydown' do |e|
        if e.keyCode.int_value == 13 # Enter
          e.preventDefault
          e.stopPropagation
          self.instance_eval(@commandLine.textContent.to_s)
          @commandLine.blur
          @aceEditor.focus
        end
      end
    end
  end

  def text_in_range(range)
    @document.getTextRange(range).to_s
  end

  def each_selected_range(&block)
    if multiple_cursors?
      @aceEditor.selection.getAllRanges.forEach(&block)
    else
      cursor = @aceEditor.selection.getCursor
      range = @aceEditor.selection.getRange
      range[:cursor] = cursor
      block[range]
    end
  end

  def multiple_cursors?
    @aceEditor.selection.ranges.length.int_value != 0
  end

  def selection?
    each_selected_range do |range|
      return true unless range.isEmpty.bool_value
    end
    return false
  end

  def eval
    results = {}

    @aceEditor.forEachSelection({ exec: JS.function do |editor|
      if editor.selection.getRange.isEmpty.bool_value
        @aceEditor.navigateLineStart
        @aceEditor.selection.selectLineEnd
      end
    end })

    each_selected_range do |range|
      begin
        results[range.end.row.int_value] = Kernel.eval(text_in_range(range)).to_s
      rescue Exception => ex
        err = ex.inspect.to_s
        JS.console.warn(err)
        results[range.end.row.int_value] = err
      end
    end

    results
  end

  def method_missing(name, *args, &block)
    @aceEditor.send(name, *args, &block)
  end
end
