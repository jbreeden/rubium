# Commands that return nil do so to prevent any insignificat result
# from being appended to the command line output.

command 'align', 'Alt-A' do
  @editor.execCommand('alignCursors')
  nil
end

command 'dup' do
  each_cursor { copyLinesDown }
  nil
end

command 'navigateUp', 'Ctrl-P' do
  each_cursor {  @editor.navigateUp(1) }
  nil
end

command 'selectUp', 'Ctrl-Shift-P' do
  each_cursor { @editor.selection.selectUp(1) }
  nil
end

command 'selectUntil' do
  each_cursor { @editor.selection.selectUp(1) }
  nil
end

command 'navigateDown', 'Ctrl-N' do
  each_cursor { @editor.navigateDown(1) }
  nil
end

command 'selectDown', 'Ctrl-Shift-N' do
  each_cursor { @editor.selection.selectDown(1) }
  nil
end

command 'navigateLeft', 'Ctrl-B' do
  each_cursor { @editor.navigateLeft(1) }
  nil
end

command 'selectLeft', 'Ctrl-Shift-B' do
  each_cursor { @editor.selection.selectLeft(1) }
  nil
end

command 'navigateRight', 'Ctrl-F' do
  each_cursor { @editor.navigateRight(1) }
  nil
end

command 'selectRight', 'Ctrl-Shift-F' do
  each_cursor { @editor.selection.selectRight(1) }
  nil
end

command 'delete', 'Ctrl-D' do
  @editor.execCommand('del')
  nil
end

command 'navigateLineStart', 'Ctrl-A' do
  each_cursor { @editor.navigateLineStart }
  nil
end

command 'selectLineStart', 'Ctrl-Shift-A' do
  each_cursor { @editor.selection.selectLineStart }
  nil
end

command 'navigateLineEnd', 'Ctrl-E' do
  each_cursor { @editor.navigateLineEnd }
  nil
end

command 'selectLineEnd', 'Ctrl-Shift-E' do
  each_cursor { @editor.selection.selectLineEnd }
  nil
end

command 'splitIntoLines', 'Ctrl-Shift-L' do
  @editor.selection.splitIntoLines
  nil
end

command 'undo', 'Ctrl-Z' do
  @editor.undo
  nil
end

command 'redo', 'Ctrl-Shift-Z' do
  @editor.redo
  nil
end

command 'switchPanels', 'Ctrl-Tab' do
  @model.rotate_panel_focus
  nil
end

command 'reload' do
  JS.window.location.reload
end

command "runSelections", "Ctrl-R" do
  maintain_user_state do
    self.eval
  end
  nil
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
  nil
end

command 'sort' do
  contents = []
  each_range do |range|
    contents.push(get_text_range(range))
  end

  contents.sort!

  i = 0
  each_range_in_order do |range|
    @document.replace(range, contents[i])
    i += 1
  end

  nil
end

command 'shuffle' do
  contents = []
  each_range do |range|
    contents.push(get_text_range(range))
  end
  # For 2 elements, guarantee a swap
  if contents.length == 2
    contents = [contents[1], contents[0]]
  else
    contents.shuffle!
  end
  i = 0
  each_range_in_order do |range|
    @document.replace(range, contents[i])
    i += 1
  end

  nil
end
