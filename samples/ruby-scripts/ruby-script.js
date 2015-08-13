(function () {
  var proto = Object.create(HTMLElement.prototype);

  proto.createdCallback = function () {
    this.style.display = 'none';
  };

  proto.attachedCallback = function () {
    var self = this;
    if (self.getAttribute('onready')) {
      document.addEventListener('DOMContentLoaded', function () {
        ruby(self.textContent);
      });
    }
    else if (self.getAttribute('onload')) {
      document.addEventListener('load', function () {
        ruby(self.textContent);
      });
    }
    else {
      ruby(self.textContent);
    }
  };

  var RubyScript = document.registerElement('ruby-script', {
    prototype: proto
  });
}());
