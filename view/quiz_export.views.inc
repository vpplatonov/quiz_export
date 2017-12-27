<?php

/**
 * @file
 * Views hooks & handlers.
 */

/**
 * Implements hook_views_data().
 */
function quiz_export_views_data() {
  return array(
    'views' => array(
      'quiz_export_link' => array(
        'title' => t('Quiz export download link'),
        'help' => t('Add Quiz export download link if content type "Quiz"'),
        'group' => 'Content',
        'field' => array(
          'handler' => 'quiz_export_views_handler_field_download_link',
          'click sortable' => FALSE,
        ),
      ),
    )
  );
}

/**
 * Implements hook_views_handlers().
 */
function quiz_export_views_handlers() {
  return array(
    'handlers' => array(
      'quiz_export_views_handler_field_download_link' => array(
        'parent' => 'views_handler_field',
        'file' => 'quiz_export_views_handler_field_download_link.inc',
      ),
    ),
  );
}

