module.exports = function(app) {
  var file = require('../../../libraries/file')
    , _ = require('underscore');

  app.get('/api/t/:id', function(req, res) {
    var id = req.param('id')
      , locale = req.param('l')
      , translations = file.readTranslations(locale, { returnType : 'array' })
      , translation = _.findWhere(translations, { id : id })
    res.json(translation);
  });

  app.put('/api/t/:id', function(req, res) {
    var id = req.param('id')
      , locale = req.cookies.gt_locale || cfg.DEFAULT_LOCALE
      , translations = file.readTranslations(locale, { returnType : 'array' })
      , translation = req.body;

    translations = translations.map(function(_translation) {
      if(_translation.id === id) {
        return translation;
      }
      else {
        return _translation;
      }
    });

    file.writeSingleLocaleTranslations(translations, locale, function() {
      res.json(translation);
    });
  });
};
