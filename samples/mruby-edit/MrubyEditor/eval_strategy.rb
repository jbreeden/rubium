module MrubyEditor::EvalStrategy
  def eval
    results = {}

    each_cursor do
      if @editor.selection.getRange.isEmpty.bool_value
        @editor.navigateLineStart
        @editor.selection.selectLineEnd
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
end
