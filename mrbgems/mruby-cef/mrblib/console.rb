module Cef::Console
  def self.log(*msg)
    msg = msg.join ' '
    Cef::V8.window['console']['log'].apply(Cef::V8.window['console'], [Cef::V8.create_string(msg)])
  end
end
