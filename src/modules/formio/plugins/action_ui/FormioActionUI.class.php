<?php

interface FormioActionUIInterface {

  function init($plugin);
  function action_forms(&$actions, $args);
  function edit_action_form(&$form, &$form_state);
}
class FormioActionUI implements FormioActionUIInterface {
  var $plugin;
  var $name;
  var $action_info = array();

  function init($plugin) {
    $this->plugin = $plugin;
  }

  /**
   * Implements action_forms().
   */
  function action_forms(&$actions, $args) {
    return array(
      'default:action' => array(
        'title' => t('Form.io Submission'),
        'form' => 'formio_submission_form',
      ),
    );
  }

  function edit_action_form(&$form, &$form_state) {
//    $class = ctools_plugin_get_class($plugin, 'ctools_export_ui');
    $handler = ctools_plugin_load_class('ctools', 'export_ui', 'ctools_export_ui', 'ctools_export_ui');
    $edit_form = call_user_func_array(array($handler, __FUNCTION__), array($form, $form_state));
  }
}
