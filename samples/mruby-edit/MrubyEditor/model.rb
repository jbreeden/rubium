class MrubyEditor::Model
  extend Properties
  extend Events

  property :restoring_editor_state, false
  property :file_name, nil
  property :script, ""
  property :cwd, Dir.pwd
  property :last_command_result, nil
  property :command_prompt, "ruby>"
  property :command_line, ""
  property :command_line_output, ""
  property :terminal_mode, :ruby
  property :rotating_panel_focus, :false
  property :focused_panel, :editor

  event    :command_invoked
  event    :command_completed

  def initialize
    @terminal_modes = [
      :ruby,
      :js,
      :shell
    ]

    @panel_focus_order = [
      :editor,
      :command_line
    ]

    self.terminal_mode.subscribe(:immediate) do |new_value|
      self.command_prompt.set "#{new_value}>"
    end

    self.focused_panel.subscribe do |panel|
      if @panel_focus_order[0] != panel
        raise "Unrecognized panel #{panel}" unless @panel_focus_order.include?(panel)
        i = @panel_focus_order.index(panel)
        @panel_focus_order = @panel_focus_order[i..(@panel_focus_order.length)].concat(@panel_focus_order[0...i])
      end
    end
  end

  def rotate_terminal_mode
    @terminal_modes.push(@terminal_modes.shift)
    terminal_mode.set(@terminal_modes[0])
  end

  def rotate_panel_focus
    @panel_focus_order.push(@panel_focus_order.shift)
    focused_panel.set(@panel_focus_order[0])
  end
end
