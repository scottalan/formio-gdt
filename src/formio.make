api = 2
core = 7.x

defaults[projects][subdir] = "contrib"
defaults[projects][type] = "module"

;; Project-specific Dependencies
projects[views_bulk_operations][version] = "3.3"

projects[admin_views][version] = "1.5"

projects[ctools][version] = "1.9"
projects[ctools][patches][0] = "https://www.drupal.org/files/issues/ctools-readd_access_callback_params-2209775-24.patch"
projects[ctools][patches][1] = "https://www.drupal.org/files/issues/views_content-keyword-substitution-1910608-33.patch"
projects[ctools][patches][2] = "https://www.drupal.org/files/issues/1901106-ctools-views_content-override-ajax-34.patch"
projects[ctools][patches][3] = "http://drupal.org/files/2023705-ctools-autosubmit-2_0.patch"
projects[ctools][patches][4] = "https://www.drupal.org/files/issues/2448989-ctools-from_field_children-8.patch"
projects[ctools][patches][5] = "https://www.drupal.org/files/issues/ctools-more_than_one_comment_pager-2483415-1.patch"
projects[ctools][patches][6] = "https://www.drupal.org/files/issues/ctools-n2493041-3.patch"
projects[ctools][patches][7] = "https://www.drupal.org/files/issues/2555469-ctools-comments-node_tag_new-2.patch"

projects[devel][version] = "1.5"

projects[entity][version] = "1.6"
projects[entity][patches][0] = "http://drupal.org/files/entity-translatable_fields_not_overriding_und_with_empty_values-1782134-5.patch"
projects[entity][patches][1] = "http://drupal.org/files/issues/entity-1788568-21-entity_metadata_wrapper_revisions.patch"
projects[entity][patches][2] = "http://drupal.org/files/issues/entity_unsupported_operand-2407905-1.patch"
projects[entity][patches][3] = "https://www.drupal.org/files/issues/add_create_op_to_metadata_comment_access-2236229-1.patch"
projects[entity][patches][4] = "https://www.drupal.org/files/issues/entity_undefined_entity_get_info-2289693-2.patch"
projects[entity][patches][5] = "http://drupal.org/files/entity-translatable_fields_not_overriding_und_with_empty_values-1782134-5.patch"
projects[entity][patches][6] = "http://drupal.org/files/issues/entity-1788568-21-entity_metadata_wrapper_revisions.patch"
projects[entity][patches][7] = "http://drupal.org/files/issues/entity_unsupported_operand-2407905-1.patch"
projects[entity][patches][8] = "https://www.drupal.org/files/issues/add_create_op_to_metadata_comment_access-2236229-1.patch"
projects[entity][patches][9] = "https://www.drupal.org/files/issues/entity_undefined_entity_get_info-2289693-2.patch"

projects[panels][version] = "3.5"
projects[panels][patches][0] = "https://www.drupal.org/files/issues/panels-export-indentation-2448825-1.patch"
projects[panels][patches][1] = "https://www.drupal.org/files/issues/panels-focus-add-content-tab-2390803-13.patch"
projects[panels][patches][2] = "https://www.drupal.org/files/issues/panels-1588212-10.patch"

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

projects[paragraphs][version] = "1.0-rc4"
projects[paragraphs][patches][0] = "https://www.drupal.org/files/issues/paragraphs-instructions_setting-2458801-9.patch"
projects[paragraphs][patches][1] = "https://www.drupal.org/files/issues/paragraphs-modal_targets_wrong_id-2481627-3.patch"
projects[paragraphs][patches][2] = "https://www.drupal.org/files/issues/2560601-paragraphs-join_extra-2.patch"

projects[restful][version] = "2.7"

projects[views][version] = "3.11"
projects[views][patches][0] = "http://drupal.org/files/views-exposed-sorts-2037469-1.patch"
projects[views][patches][1] = "http://drupal.org/files/issues/views-ajax-nginx-1036962-71.patch"
projects[views][patches][2] = "http://drupal.org/files/1979926-views-reset_fetch_data-2.patch"
projects[views][patches][3] = "https://www.drupal.org/files/issues/1735096-views-multiple-instance-exposed-form-15.patch"
projects[views][patches][4] = "https://www.drupal.org/files/issues/2411922-views-group_name-3.patch"
projects[views][patches][5] = "https://www.drupal.org/files/issues/2473389-views-exta-args-3.patch"
projects[views][patches][6] = "http://drupal.org/files/views-exposed-sorts-2037469-1.patch"
projects[views][patches][7] = "http://drupal.org/files/issues/views-ajax-nginx-1036962-71.patch"
projects[views][patches][8] = "http://drupal.org/files/1979926-views-reset_fetch_data-2.patch"
projects[views][patches][9] = "https://www.drupal.org/files/issues/1735096-views-multiple-instance-exposed-form-15.patch"
projects[views][patches][10] = "https://www.drupal.org/files/issues/2411922-views-group_name-3.patch"
projects[views][patches][11] = "https://www.drupal.org/files/issues/2473389-views-exta-args-3.patch"


;;; Theme
projects[bootstrap][version] = "3.5"
projects[bootstrap][type] = "theme"
