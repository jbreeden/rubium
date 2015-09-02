module MrubyEditor::StateManagement
  def save_editor_state
    JS.localStorage.script = @editor.getValue
    JS.localStorage.commandLine = @model.command_line.get
  end

  def restore_editor_state
    @model.restoring_editor_state.set true
    unless JS.localStorage.script.is_undefined?
      @model.script.set JS.localStorage.script
      @editor.navigateFileStart
    end

    unless JS.localStorage.commandLine.is_undefined?
      @model.command_line.set JS.localStorage.commandLine
    end
    @model.restoring_editor_state false
  end

  def save_user_state
    @user_state[:scroll_top] = @session.getScrollTop
  end

  def save_state
    save_editor_state
    save_user_state
  end

  def restore_user_state
    @session.setScrollTop @user_state[:scroll_top] if @user_state[:scroll_top]
  end

  def maintain_user_state(&block)
    save_state # editor & user states (so we can restore editor in case of crash)
    block[]
    restore_user_state
  end
end
