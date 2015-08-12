(function () {
  window.rubium = window.rubium || {};

  rubium.register = function(elementName) {
    var camelCaseName = elementName.split('-')
      .map(function(str) {
        return str[0].toUpperCase() + str.slice(1, str.length);
      })
      .join('');

    var registerFunctionName = 'register' + camelCaseName + 'Element';

    if (typeof rubium[registerFunctionName] != 'function') {
      throw 'Attempted to register undefined element ' + elementName;
    } else {
      rubium[registerFunctionName]();
    }
  }


}());
