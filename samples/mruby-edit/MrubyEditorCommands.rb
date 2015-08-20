command 'navigateUp', 'Ctrl-P' do
  each_cursor {  @editor.navigateUp(1) }
end

command 'selectUp', 'Ctrl-Shift-P' do
  each_cursor { @editor.selection.selectUp(1) }
end

command 'selectUntil' do
  each_cursor { @editor.selection.selectUp(1) }
end

command 'navigateDown', 'Ctrl-N' do
  each_cursor { @editor.navigateDown(1) }
end

command 'selectDown', 'Ctrl-Shift-N' do
  each_cursor { @editor.selection.selectDown(1) }
end

command 'navigateLeft', 'Ctrl-B' do
  each_cursor { @editor.navigateLeft(1) }
end

command 'selectLeft', 'Ctrl-Shift-B' do
  each_cursor { @editor.selection.selectLeft(1) }
end

command 'navigateRight', 'Ctrl-F' do
  each_cursor { @editor.navigateRight(1) }
end

command 'selectRight', 'Ctrl-Shift-F' do
  each_cursor { @editor.selection.selectRight(1) }
end

command 'navigateLineStart', 'Ctrl-A' do
  each_cursor { @editor.navigateLineStart }
end

command 'selectLineStart', 'Ctrl-Shift-A' do
  each_cursor { @editor.selection.selectLineStart }
end

command 'navigateLineEnd', 'Ctrl-E' do
  each_cursor { @editor.navigateLineEnd }
end

command 'selectLineEnd', 'Ctrl-Shift-E' do
  each_cursor { @editor.selection.selectLineEnd }
end

command 'splitIntoLines', 'Ctrl-Shift-L' do
  @editor.selection.splitIntoLines
end

command 'undo', 'Ctrl-Z' do
  @editor.undo
end

command 'redo', 'Ctrl-Shift-Z' do
  @editor.redo
end

command 'align' do
  max_col = 0
  each_cursor do
    cursor = @editor.selection.getCursor
    col = cursor.column.int_value
    max_col = (max_col > col ? max_col : col)
  end
  each_cursor do
    cursor = @editor.selection.getCursor
    col = cursor.column.int_value
    if col < max_col
      @document.insert cursor, (' ' * (max_col - col))
    end
  end
end

command 'dup' do
  each_cursor { copyLinesDown }
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

def theme(name)
  @editor.setTheme("ace/theme/#{name}")
end

def mode(name)
  @session.setMode("ace/mode/#{name}")
end

def fopen(path, opt = {})
  if File.exists?(path)
    @editor.setValue(File.read(path))
    if opt[:mode]
      mode opt[:mode]
    else
      @file_modes.keys.each do |ext|
        if path.end_with?(ext)
          mode @file_modes[ext]
          break
        end
      end
    end
  else
    @editor.setValue("File not found: #{path}")
  end
end

def sh(cmd, mode = :insert)
  result = "`#{cmd}`\n#{`#{cmd}`}"
  if mode == :replace
    @editor.setValue result
  else
    each_cursor { @editor.insert result }
  end
end
