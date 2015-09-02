JS.window[:MrubyEditor] = JS.function do |selector|
  $ed = MrubyEditor.new(selector)
end

class MrubyEditor
  load './MrubyEditor/ace_adapters.rb'
  load './MrubyEditor/view.rb'
  load './MrubyEditor/file_modes.rb'
  load './MrubyEditor/eval_strategy.rb'
  load './MrubyEditor/state_management.rb'
  load './MrubyEditor/properties.rb'
  load './MrubyEditor/model.rb'

  include AceAdapters
  include EvalStrategy
  include StateManagement

  def initialize(selector)
    @model = Model.new()
    @view = View.new(@model, selector)
    @editor = @view.editor
    @session = @editor.getSession
    @document = @session.doc
    self.instance_eval(File.read('./MrubyEditor/commands.rb'))
    @user_state = {}
    @editor.focus
    theme :pastel_on_dark
    mode :ruby
    setFontSize(13)

    @model.command_invoked.subscribe do
      save_editor_state
      result = nil
      begin
        case @model.terminal_mode.get
        when :ruby
          result = self.instance_eval(@model.command_line.get)
        when :shell
          result = `#{@model.command_line.get}`
        when :js
          result = JS.eval(@model.command_line.get)
        end
        @model.last_command_result.set result
      rescue Exception => ex
        @model.last_command_result.set ex.to_s
      end
      @model.cwd.set(Dir.pwd)
      @model.command_completed.trigger
    end

    restore_editor_state
    read_query_string
  end

  def read_query_string
    @query = Hash.new { |h, k| h[k] = [] }
    JS.window.location.search.to_s
      .sub("?", '')
      .split("&")
      .map { |c| JS.decodeURIComponent(c).to_s }
      .each { |param|
        if param.include?("=")
          param_name = param[0...(param.index("="))]
          param_value = param[(param.index("=") + 1)..-1]
          @query[param_name].push(param_value)
        else
          @query[param].push('')
        end
      }
  end

  def method_missing(name, *args, &block)
    @editor.send(name, *args, &block)
  end

  def theme(name)
    @editor.setTheme("ace/theme/#{name}")
  end

  def mode(name)
    @session.setMode("ace/mode/#{name}")
  end

  def insert(text)
    puts @editor.class
    each_cursor { @editor.insert(text) }
  end

  def open(path, opt = {})
    if File.exists?(path)
      @editor.setValue(File.read(path))
      if opt[:mode]
        mode opt[:mode]
      else
        @@file_modes.keys.each do |ext|
          if path.end_with?(ext)
            mode @@file_modes[ext]
            break
          end
        end
      end
      @editor.navigateFileStart
      @model.file_name.set(path)
      :success
    else
      raise "File not found: #{path}"
    end
  end
  alias fopen open

  def save(file_name = nil, force = false)
    if file_name.nil?
      open_file = @model.file_name.get
      if open_file.nil?
        raise "Please specify a file name"
      else
        File.open(open_file, 'w') do |f|
          f.write(@editor.getValue.to_s)
          :success
        end
      end
    else
      if File.exists?(file_name) && !force
        raise "File exists. Use 'save FILE_NAME, :force' to overwrite it."
      else
        File.open(file_name, 'w') do |f|
          f.write(@editor.getValue.to_s)
          :success
        end
      end
    end
  end
end
