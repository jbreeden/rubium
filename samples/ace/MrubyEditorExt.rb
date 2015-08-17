class MrubyEditor
  def replace(content)
    content = content.to_s
    @aceEditor.setValue(content)
  end

  def append(content)
    @aceEditor.setValue(JS.create_string(@aceEditor.getValue.to_s + "\n" + content.to_s))
  end

  def ofile(path)
    if File.exists?(path)
      replace(File.read(path))
    else
      replace("File not found: #{path}")
    end
  end
end
