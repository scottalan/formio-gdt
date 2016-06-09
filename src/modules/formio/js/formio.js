(function ($) {
  Drupal.behaviors.formio = {
    attach: function (context, settings) {

      // Focus on the 'Add' button for a single widget preview, after it's loaded.
      if (settings.formio && settings.formio.pane_add_preview_mode === 'single' && settings.formio.pane_add_preview_subtype) {
        // Need to defer until current set of behaviors is done, because Panels
        // will move the focus to the first widget by default.
        setTimeout(function () {
          var link_class = 'add-content-link-' + settings.formio.pane_add_preview_subtype.replace(/_/g, '-') + '-icon-text-button';
          $('#modal-content .formio-preview-link .content-type-button a.' + link_class, context).focus();
        }, 0);
      }
    }
  };

})(jQuery);

(function ($) {
  // Used to only update preview after changes stop for a second.
  var timer;

  // Used to make sure we don't wrap Drupal.wysiwygAttach() more than once.
  var wrappedWysiwygAttach = false;

  // Triggers the CTools autosubmit on the given form. If timeout is passed,
  // it'll set a timeout to do the actual submit rather than calling it directly
  // and return the timer handle.
  function triggerSubmit(form, timeout) {
    var $form = $(form),
      preview_widget = $('#formio-preview'),
      submit;
    if (!preview_widget.hasClass('formio-loading')) {
      preview_widget.addClass('formio-loading');
      submit = function () {
        $form.find('.ctools-auto-submit-click').click();
      };
      if (typeof timeout === 'number') {
        return setTimeout(submit, timeout);
      }
      else {
        submit();
      }
    }
  }

  // Used to cancel a submit. It'll clear the timer and the class marking the
  // loading operation.
  function cancelSubmit(form, timer) {
    var $form = $(form),
      preview_widget = $('#formio-preview');
    preview_widget.removeClass('formio-loading');
    clearTimeout(timer);
  }

  function onWysiwygChangeFactory(editorId) {
    return function () {
      var textarea = $('#' + editorId),
        form = textarea.get(0).form;

      if (textarea.hasClass('formio-textarea-autosubmit')) {
        // Wait a second and then submit.
        cancelSubmit(form, timer);
        timer = triggerSubmit(form, 1000);
      }
    };
  }

  // A function to run before Drupal.wysiwygAttach() with the same arguments.
  function beforeWysiwygAttach(context, params) {
    var editorId = params.field,
      editorType = params.editor,
      format = params.format;

    if (Drupal.settings.wysiwyg.configs[editorType] && Drupal.settings.wysiwyg.configs[editorType][format]) {
      wysiwygConfigAlter(params, Drupal.settings.wysiwyg.configs[editorType][format]);
    }
  }

  // Wouldn't it be great if WYSIWYG gave us an alter hook to change the
  // settings of the editor before it was attached? Well, we'll just have to
  // roll our own. :-)
  function wysiwygConfigAlter(params, config) {
    var editorId = params.field,
      editorType = params.editor,
      onWysiwygChange = onWysiwygChangeFactory(editorId);

    switch (editorType) {
      case 'markitup':
        $.each(['afterInsert', 'onEnter'], function (index, funcName) {
          config[funcName] = onWysiwygChange;
        });
        break;

      case 'tinymce':
        config['setup'] = function (editor) {
          editor.onChange.add(onWysiwygChange);
          editor.onKeyUp.add(onWysiwygChange);
        }
        break;
    }
  }

  // Used to wrap a function with a beforeFunc (we use it for wrapping
  // Drupal.wysiwygAttach()).
  function wrapFunctionBefore(parent, name, beforeFunc) {
    var originalFunc = parent[name];
    parent[name] = function () {
      beforeFunc.apply(this, arguments);
      return originalFunc.apply(this, arguments);
    };
  }

  /**
   * Improves the Auto Submit Experience for CTools Modals
   */
  Drupal.behaviors.formioAutosubmit = {
    attach: function (context, settings) {
      // Replaces click with mousedown for submit so both normal and ajax work.
      $('.ctools-auto-submit-click', context)
      // Exclude the 'Style' type form because then you have to press the
      // "Next" button multiple times.
      // @todo: Should we include the places this works rather than excluding?
        .filter(function () { return $(this).closest('form').attr('id').indexOf('panels-edit-style-type-form') !== 0; })
        .click(function(event) {
          if ($(this).hasClass('ajax-processed')) {
            event.stopImmediatePropagation();
            $(this).trigger('mousedown');
            return false;
          }
        });

      // e.keyCode: key
      var discardKeyCode = [
        16, // shift
        17, // ctrl
        18, // alt
        20, // caps lock
        33, // page up
        34, // page down
        35, // end
        36, // home
        37, // left arrow
        38, // up arrow
        39, // right arrow
        40, // down arrow
        9, // tab
        13, // enter
        27  // esc
      ];

      // Special handling for link field widgets. This ensures content which is ahah'd in still properly autosubmits.
      $('.field-widget-link-field input:text', context).addClass('formio-textfield-autosubmit').addClass('ctools-auto-submit-exclude');

      // Handle text fields and textareas.
      $('.formio-textfield-autosubmit, .formio-textarea-autosubmit', context)
        .once('ctools-auto-submit')
        .bind('keyup blur', function (e) {
          var $element;
          $element = $('.formio-preview .pane-title', context);

          cancelSubmit(e.target.form, timer);

          // Filter out discarded keys.
          if (e.type !== 'blur' && $.inArray(e.keyCode, discardKeyCode) > 0) {
            return;
          }

          // Set a timer to submit the form a second after the last activity.
          timer = triggerSubmit(e.target.form, 1000);
        });

      // Handle WYSIWYG fields.
      if (!wrappedWysiwygAttach && typeof Drupal.wysiwygAttach == 'function') {
        wrapFunctionBefore(Drupal, 'wysiwygAttach', beforeWysiwygAttach);
        wrappedWysiwygAttach = true;

        // Since the Drupal.behaviors run in a non-deterministic order, we can
        // never be sure that we wrapped Drupal.wysiwygAttach() before it was
        // used! So, we look for already attached editors so we can detach and
        // re-attach them.
        $('.formio-textarea-autosubmit', context)
          .once('formio-wysiwyg')
          .each(function () {
            var editorId = this.id,
              instance = Drupal.wysiwyg.instances[editorId],
              format = instance ? instance.format : null,
              trigger = instance ? instance.trigger : null;

            if (instance && instance.editor != 'none') {
              params = Drupal.settings.wysiwyg.triggers[trigger];
              if (params) {
                Drupal.wysiwygDetach(context, params[format]);
                Drupal.wysiwygAttach(context, params[format]);
              }
            }
          });
      }

      // Handle autocomplete fields.
      $('.formio-autocomplete-autosubmit', context)
        .once('ctools-auto-submit')
        .bind('keyup blur', function (e) {
          // Detect when a value is selected via TAB or ENTER.
          if (e.type === 'blur' || e.keyCode === 13) {
            // We defer the submit call so that it happens after autocomplete has
            // had a chance to fill the input with the selected value.
            triggerSubmit(e.target.form, 0);
          }
        });

      // Prevent ctools auto-submit from firing when changing text formats.
      $(':input.filter-list').addClass('ctools-auto-submit-exclude');

    }
  }
})(jQuery);
