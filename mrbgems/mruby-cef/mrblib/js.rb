JS = Cef::V8

module JS
  class << JS
    alias legacy_eval eval
    def eval(str)
      legacy_eval(str) do |exc|
        raise exc.message
      end
    end
  end

  def self.type_of(js_object)
    if js_object.is_undefined?
      'undefined'
    elsif js_object.is_null?
      'null'
    elsif js_object.is_bool?
      'bool'
    elsif js_object.is_int?
      'int'
    elsif js_object.is_uint?
      'uint'
    elsif js_object.is_double?
      'double'
    elsif js_object.is_date?
      'date'
    elsif js_object.is_string?
      'string'
    elsif js_object.is_array?
      'array'
    elsif js_object.is_function?
      'function'
    elsif js_object.is_object?
      'object'
    end
  end

  def console
    JS.window[:console]
  end

  def document
    JS.window[:document]
  end

  def localStorage
    JS.window[:localStorage]
  end

  def self.function(&block)
    raise "Must supply a block to JS.function" unless block
    JS.create_function("anonymous") do |args|
      JS.arguments = args
      block[*args]
    end
  end

  def self.method_missing(name, *args, &block)
    JS.window.send(name, *args, &block)
  end
end

class JS::JsObject
  def to_s
    # NOPE: Remember, primitives are not really objects in JS.
    #       When you do `"some string literal".toString()` JS creates a temporary `String`
    #       (notice the uppercase) object and calls `toString` on that
    # self.toString.to_s <= WRONG
    JS[:String].apply(JS.window, [self]).string_value
  end

  def to_js
    self
  end

  def method_missing(name, *args, &block)
    # `self` must be an object for property access to work
    unless self.is_object?
      raise "Cannot read property #{name} of #{JS.type_of(self)}"
    end

    # We're calling a JS function from Ruby, so convert the arguments if possible
    args = args.map { |arg|
      if arg.kind_of? JsObject
        arg
      else
        arg.to_js
      end
    }

    name_str = name.to_s

    # Setter
    if name_str.end_with? '='
      self[name_str[0...-1]] = args[0]
    # Function invocation
    elsif self[name].is_function?
      if block.nil?
        self[name].apply(self, args)
      else
        # If a block was supplied, pass it as a function in the last argument to the called function
        func = JS.create_function 'anonymous' do |args|
          JS.arguments = args
          block[*args]
        end
        args.push(func)
        self[name].apply(self, args) do |exc|
          $stderr.puts "WARNING: JavaScript function threw #{exc.message}"
          raise exc.message
        end
      end
    # No arguments, just return the field (may be undefined, just as in JS)
    elsif args.length == 0
      self[name]
    # Arguments must have been supplied, but this isn't a function
    else
      raise "#{JS.type_of(self[name])} is not a function"
    end
  end
end

class Object
  def to_js
    raise "No implicit conversion of #{self.class.to_s} to JsObject"
  end
end

class NilClass
  def to_js
    JS.create_null
  end
end

class Fixnum
  def to_js
    JS.create_int(self)
  end
end

class Float
  def to_js
    JS.create_float(self)
  end
end

class TrueClass
  def to_js
    JS.create_bool(self)
  end
end

class FalseClass
  def to_js
    JS.create_bool(self)
  end
end

class String
  def to_js
    JS.create_string(self)
  end
end

class Symbol
  def to_js
    JS.create_string(self.to_s)
  end
end

class Array
  def to_js
    result = JS.eval('[]')
    self.each do |el|
      result.push(el.to_js)
    end
    result
  end
end

class Hash
  def to_js
    result = JS.create_object
    self.each do |key, value|
      result[key] = value.to_js
    end
    result
  end
end

def define_ruby_js_function
  ruby = JS.create_function 'ruby' do |args|
    result = eval(args[0].string_value)
    begin
      # TODO: Detect & resolve reference cycles when converting
      #       Ex: Array has an element that references itself.
      result = result.kind_of?(JS::JsObject) ? result : result.to_js
    rescue Exception => ex
      # Couldn't convert return value to JsObject. Just return undefined.
      # (Note that everything in Ruby returns something, so this may be unintential - not an error)
      JS.console.warn("Failed to convert return value to JavaScript object: #{result.to_s}")
      result = JS.create_undefined
    end
    result
  end

  JS.window.ruby = ruby
end
