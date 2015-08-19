command 'navigateUp', 'Ctrl-P' do
  each_cursor {  @ace.navigateUp(1) }
end

command 'selectUp', 'Ctrl-Shift-P' do
  each_cursor { @ace.selection.selectUp(1) }
end

command 'selectUntil' do
  each_cursor { @ace.selection.selectUp(1) }
end

command 'navigateDown', 'Ctrl-N' do
  each_cursor { @ace.navigateDown(1) }
end

command 'selectDown', 'Ctrl-Shift-N' do
  each_cursor { @ace.selection.selectDown(1) }
end

command 'navigateLeft', 'Ctrl-B' do
  each_cursor { @ace.navigateLeft(1) }
end

command 'selectLeft', 'Ctrl-Shift-B' do
  each_cursor { @ace.selection.selectLeft(1) }
end

command 'navigateRight', 'Ctrl-F' do
  each_cursor { @ace.navigateRight(1) }
end

command 'selectRight', 'Ctrl-Shift-F' do
  each_cursor { @ace.selection.selectRight(1) }
end

command 'navigateLineStart', 'Ctrl-A' do
  each_cursor { @ace.navigateLineStart }
end

command 'selectLineStart', 'Ctrl-Shift-A' do
  each_cursor { @ace.selection.selectLineStart }
end

command 'navigateLineEnd', 'Ctrl-E' do
  each_cursor { @ace.navigateLineEnd }
end

command 'selectLineEnd', 'Ctrl-Shift-E' do
  each_cursor { @ace.selection.selectLineEnd }
end

command 'splitIntoLines', 'Ctrl-Shift-L' do
  @ace.selection.splitIntoLines
end

command 'undo', 'Ctrl-Z' do
  @ace.undo
end

command 'redo', 'Ctrl-Shift-Z' do
  @ace.redo
end

command 'align' do
  max_col = 0
  each_cursor do
    cursor = @ace.selection.getCursor
    col = cursor.column.int_value
    max_col = (max_col > col ? max_col : col)
  end
  each_cursor do
    cursor = @ace.selection.getCursor
    col = cursor.column.int_value
    if col < max_col
      @document.insert cursor, (' ' * (max_col - col))
    end
  end
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
end

command 'shuffle' do
  contents = []
  each_range do |range|
    contents.push(get_text_range(range))
  end
  contents.shuffle!
  i = 0
  each_range_in_order do |range|
    @document.replace(range, contents[i])
    i += 1
  end
end
