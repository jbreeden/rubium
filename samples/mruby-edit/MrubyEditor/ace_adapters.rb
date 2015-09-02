module MrubyEditor::AceAdapters
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

  def multiple_cursors?
    @editor.selection.ranges.length.int_value != 0
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
    @editor.forEachSelection({ exec: JS.function(&block) })
  end

  def each_range(&block)
    unless block
      return enum_for(:each_range)
    end
    if multiple_cursors?
      @editor.selection.getAllRanges.forEach(&block)
    else
      cursor = @editor.selection.getCursor
      range = @editor.selection.getRange
      # With getAllRanges, each range has a cursor reference.
      # This normalizes the single range case so that all calls are the same.
      range[:cursor] = cursor
      block[range]
    end
  end

  def each_range_in_order(&block)
    ranges = []
    each_range do |r|
      ranges.push(r)
    end
    ranges.sort_by { |r| [r.start.row.int_value, r.start.column.int_value] }.each(&block)
  end
end
