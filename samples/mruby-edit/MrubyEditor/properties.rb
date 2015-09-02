class Property
  def initialize(val=nil)
    @listeners = []
    @val = val
  end

  def get
    @val
  end

  def set(val)
    prev = @val
    @val = val
    if prev != @val
      trigger(@val)
    end
  end

  def subscribe(immediate = false, &block)
    raise "No block provided" if block.nil?
    unless @listeners.include?(block)
      @listeners.unshift(block)
    end
    block[@val] if immediate
  end

  def unsubscribe(&block)
    raise "No block provided" if block.nil?
    @listeners.delete(block)
  end

  def trigger(new_val)
    @listeners.dup.each do |p|
      p[new_val]
    end
  end
end

module Properties
  def property(name, initial_value = nil)
    self.define_method(name) do
      self.instance_variable_set(
        "@#{name}",
        self.instance_variable_get("@#{name}") || Property.new(initial_value)
      )
    end
  end
end

class Event
  def initialize
    @listeners = []
  end

  def subscribe(&block)
    raise "No block provided" if block.nil?
    unless @listeners.include?(block)
      @listeners.unshift(block)
    end
  end
  alias add subscribe

  def unsubscribe(&block)
    raise "No block provided" if block.nil?
    @listeners.delete(block)
  end
  alias remove unsubscribe
  alias delete unsubscribe

  def trigger(data = nil)
    @listeners.dup.each do |p|
      p[data]
    end
  end
end


module Events
  def event(name)
    self.define_method(name) do
      self.instance_variable_set(
        "@#{name}",
        self.instance_variable_get("@#{name}") || Event.new
      )
    end
  end
end
