<?php

class FormioEntityCreateUI extends FormioActionUI {

  /**
   * Implements init().
   *
   * Initialization method for the plugin.
   */
  function init($plugin) {
    $this->name = $plugin['name'];
    parent::init($plugin);
  }

  function action_forms(&$actions, $args) {
    return array(
      'entity:action' => array(
        'title' => t('Entity Type'),
        'form' => 'formio_entity_type_action_form',
      ),
      'entity:bundle' => array(
        'title' => t('Bundle'),
        'form' => 'formio_entity_bundle_action_form',
      ),
      'entity:field_map' => array(
        'title' => t('Map Fields'),
        'form' => 'formio_entity_field_map_action_form',
      ),
    );
  }

  function edit_action_form(&$form, &$form_state) {
    $steps = $this->plugin['steps'];
    parent::edit_action_form($form, $form_state);
  }

  function formio_entity_type_action_form(&$form, &$form_state) {

  }

}
