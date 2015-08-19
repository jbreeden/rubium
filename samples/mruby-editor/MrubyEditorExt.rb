class MrubyEditor
  def ofile(path)
    if File.exists?(path)
      @ace.setValue(File.read(path))
    else
      @ace.setValue("File not found: #{path}")
    end
  end

  def sh(cmd, mode = :insert)
    result = "`#{cmd}`\n#{`#{cmd}`}"
    if mode == :replace
      @ace.setValue result
    else
      each_cursor { @ace.insert result }
    end
  end
end
