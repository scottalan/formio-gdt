api = 2
core = 7.x

defaults[projects][subdir] = "contrib"
defaults[projects][type] = "module"

;; Project-specific Dependencies

projects[admin_views][version] = "1.5"

projects[ctools][version] = "1.9"
projects[ctools][patch][0] = "https://www.drupal.org/files/issues/ctools-readd_access_callback_params-2209775-24.patch"
projects[ctools][patch][1] = "https://www.drupal.org/files/issues/views_content-keyword-substitution-1910608-33.patch"
projects[ctools][patch][2] = "https://www.drupal.org/files/issues/1901106-ctools-views_content-override-ajax-34.patch"
projects[ctools][patch][3] = "http://drupal.org/files/2023705-ctools-autosubmit-2_0.patch"
projects[ctools][patch][4] = "https://www.drupal.org/files/issues/2448989-ctools-from_field_children-8.patch"
projects[ctools][patch][5] = "https://www.drupal.org/files/issues/ctools-more_than_one_comment_pager-2483415-1.patch"
projects[ctools][patch][6] = "https://www.drupal.org/files/issues/ctools-n2493041-3.patch"
projects[ctools][patch][7] = "https://www.drupal.org/files/issues/2555469-ctools-comments-node_tag_new-2.patch"

projects[defaultconfig][version] = "1.0-alpha11"

projects[devel][version] = "1.5"

projects[entity][version] = "1.6"
projects[entity][patch][0] = "http://drupal.org/files/entity-translatable_fields_not_overriding_und_with_empty_values-1782134-5.patch"
projects[entity][patch][1] = "http://drupal.org/files/issues/entity-1788568-21-entity_metadata_wrapper_revisions.patch"
projects[entity][patch][2] = "http://drupal.org/files/issues/entity_unsupported_operand-2407905-1.patch"
projects[entity][patch][3] = "https://www.drupal.org/files/issues/add_create_op_to_metadata_comment_access-2236229-1.patch"
projects[entity][patch][4] = "https://www.drupal.org/files/issues/entity_undefined_entity_get_info-2289693-2.patch"
; @see https://github.com/RESTful-Drupal/restful#restful-best-practices-for-drupal
projects[entity][patch][5] = "https://www.drupal.org/files/issues/2086225-entity-access-check-node-create-3.patch"

projects[features][version] = 2.8

projects[jquery_update][version] = "2.7"

projects[libraries][version] = "2.2"

projects[navbar][version] = "1.7"

projects[panelizer][version] = 3.x-dev
projects[panelizer][subdir] = contrib
projects[panelizer][download][type] = git
projects[panelizer][download][branch] = 7.x-3.x
projects[panelizer][download][revision] = 6b76359
projects[panelizer][patch][2514068] = https://www.drupal.org/files/issues/2514068-panelizer-cache_key-2.patch
projects[panelizer][patch][1549608] = https://www.drupal.org/files/issues/1549608-panelizer-cache-24.patch

projects[panels][version] = "3.5"
projects[panels][patch][0] = "https://www.drupal.org/files/issues/panels-export-indentation-2448825-1.patch"
projects[panels][patch][1] = "https://www.drupal.org/files/issues/panels-focus-add-content-tab-2390803-13.patch"
projects[panels][patch][2] = "https://www.drupal.org/files/issues/panels-1588212-10.patch"

projects[paragraphs][version] = "1.0-rc4"
projects[paragraphs][patch][0] = "https://www.drupal.org/files/issues/paragraphs-instructions_setting-2458801-9.patch"
projects[paragraphs][patch][1] = "https://www.drupal.org/files/issues/paragraphs-modal_targets_wrong_id-2481627-3.patch"
projects[paragraphs][patch][2] = "https://www.drupal.org/files/issues/2560601-paragraphs-join_extra-2.patch"

;;;;; JUST FOR FUN ;;;;;
projects[plug][version] = 1.1
projects[plug_config][version] = 1.x-dev
projects[plug_field][version] = 1.x-dev

projects[registry_autoload][version] = "1.3"

projects[restful][version] = "2.7"

projects[strongarm][version] = "2.0"

projects[views][version] = "3.11"
projects[views][patch][0] = "http://drupal.org/files/views-exposed-sorts-2037469-1.patch"
projects[views][patch][1] = "http://drupal.org/files/issues/views-ajax-nginx-1036962-71.patch"
projects[views][patch][2] = "http://drupal.org/files/1979926-views-reset_fetch_data-2.patch"
projects[views][patch][3] = "https://www.drupal.org/files/issues/1735096-views-multiple-instance-exposed-form-15.patch"
projects[views][patch][4] = "https://www.drupal.org/files/issues/2411922-views-group_name-3.patch"
projects[views][patch][5] = "https://www.drupal.org/files/issues/2473389-views-exta-args-3.patch"

projects[views_bulk_operations][version] = "3.3"

projects[webform][version] = "4.12"

;;; Theme
projects[bootstrap][version] = "3.5"
projects[bootstrap][type] = "theme"
