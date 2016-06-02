<?php
/**
 * @file
 */

/**
 * Defining your own action plugin is super simple!
 *
 * You'll see this is just a standard implementation of ctools plugins.
 *
 * Below, is a basic example of all you need to define a new action,
 * get the action to show up when creating a preset, and executing the action
 * on form submission.
 */

// First we'll define our basic action plugin.
$plugin = array(
  // Give the action a title. This title is used in the preset on the action
  // selection page of the wizard.
  'title' => t('My Action'),

  // You don't have to define a handler but if you do, you do not have to
  // define the 'action forms' array below. If you have both, as you see here
  // then the 'handler' wins and it will expect your 'MyActionUI' class to
  // define a method, 'action_forms', that returns the same array you see
  // below.
  'handler' => array(
    'class' => 'MyActionUI',
  ),

  // The order of the forms in this array will be the order in which
  // they are loaded in the preset wizard.
  //
  // The naming of the 'key' for each of the steps has some significance:
  //   1. It becomes the 'step' you are on in the wizard.
  //   2. It's used to include your plugin so that the $handler['class']
  //      is loaded and the correct method is found.
  //
  // The naming convention is as follows:
  //   - module:action
  //
  // An example might be:
  //   - formio:entity
  //
  // This will tell the preset to look for an action in the formio module.
  //   - plugins
  //     - actions
  //       - entity.inc
  //
  // @todo: Figure out what the significance of the key is here - action:form:1
  'action forms' => array(
    // This is the step and title.
    'mymodule:myaction' => t('My Action'),
  ),
);

/**
 * Now that we've defined the $plugin we just need to write our "form"
 * function that will provide the form we need. In your module's 'plugin'
 * directory create a directory: 'actions'. In that directory create a file:
 * 'my_action.inc'.
 *
 * This is an example of what you might have in that file.
 */
$plugin = array(
  'title' => t('My Action'),

  'handler' => array(
    'class' => 'MyAction',
  ),

  'has menu' => FALSE,
  'access' => TRUE,

);

/**
 * You can either add the class to the same file and add that class to the
 * files array: files[] = plugins/actions/entity.inc or you can create a
 * separate file: MyAction.class.php
 */
class MyAction extends FormioActions implements FormioActionsInterface {

  function init($plugin) {
    parent::init($plugin);
  }

  function hook_menu(&$items) {
    // Define a custom menu item that will be used to call your callback
    // function.

    // This is needed as it adds a file and file path to that file for the menu.
    parent::hook_menu($items);
  }

  function action($plugin, $preset_name, $export) {

    // This is where you would add any logic needed to handle your custom
    // 'action' in Drupal.

    // The parent action will handle submitting the form to your project on
    // form.io.
    parent::action($plugin);
  }
}

/**
 * For every form that you define as an export, a block is automatically
 * created.
 */
function hook_block_info() {

}
