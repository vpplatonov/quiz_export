<?php

/**
 * @File
 * API export QUIZ to excel file format.
 * There is in QUIZ 7.x-4.0 version module quiz is node (type="quiz") with additional fields.
 */

/**
 * Menu callback export quiz to excel
 *
 * @param int $quiz_id
 *   The class id.
 *
 * @return void
 *   Returns export data.
 */
function quiz_export_excel($quiz_id) {
  $library = libraries_load('PHPExcel');

  if (!$library) {
    drupal_not_found();

    return;
  }

  // Load Quiz
  $quiz = node_load($quiz_id);
  if (!$quiz || $quiz->type != QUIZ_NODE_TYPE) {

    return;
  }

  try {
    $excel = quiz_export_excel_build($quiz);
    $writer = PHPExcel_IOFactory::createWriter($excel, QUIZ_EXPORT_EXCEL_TYPE);

    $file_name = $quiz->title . ' ' . format_date(REQUEST_TIME, 'custom', QUIZ_EXPORT_DATA_FORMAT);
    quiz_export_file_header($file_name . '.' . QUIZ_EXPORT_EXCEL_EXT);

    // Force the file to download.
    $writer->save('php://output');
  }
  catch (PHPExcel_Exception $e) {
    watchdog('quiz_export', $e->getMessage());
  }
}

/**
 * Helper for build Excel object.
 *
 * @param object $quiz
 *   Quiz object
 * @param bool $strip_tags
 *   Remove HTML Tags from text & html_entity_decode().
 * @param bool $field_name_inrow
 *   Field name in first row for question type for multiple answers.
 *
 * @return \PHPExcel
 * @throws \PHPExcel_Exception
 */
function quiz_export_excel_build($quiz, $strip_tags = TRUE, $field_name_inrow = FALSE) {
  $excel = new PHPExcel();
  $excel->setActiveSheetIndex(0);

  // Fill the header.
  foreach (quiz_export_xls_mapper() as $col => $label) {
    $excel->getActiveSheet()->setCellValue("{$col}1", $label);
    $excel->getActiveSheet()->getColumnDimension($col)->setAutoSize(TRUE);
    $excel->getActiveSheet()->getStyle("{$col}1")->getFont()->setBold(TRUE);
  }

  // Fill data.
  $questions = quiz_export_quiz_questions($quiz);
  $answer_type_map = quiz_export_answer_properties_map();

  $row = 2;
  foreach ($questions as $q) {
    // Get Question structure
    $description = $strip_tags
      ? trim(strip_tags(html_entity_decode($q->description, ENT_QUOTES | ENT_HTML401)))
      : $q->description;
    unset($q->type);
    _quiz_question_get_instance($q);

    // Prepare data
    $data = array(
      $q->type,
      $q->title,
      $description,
    );

    foreach ($answer_type_map[$q->type] as $answer_map) {
      if ($q->type == 'scale') {
        $q->answer = quiz_export_scale_answers($q);
      }

      if (is_array($q->{$answer_map})) {
        if (!is_array(reset($q->{$answer_map}))) {
          // It's not possible but: we don't stop processing.
          $q->{$answer_map}[] = array();
        }

        // save keys
        $answer_type_fields = array_keys(reset($q->{$answer_map}));

        // Remove unnecessary fields like id & format.
        $unset_keys = quiz_export_clean_properties($answer_type_fields, $q->type);

        if ($field_name_inrow) {
          $data = array_merge($data, $answer_type_fields);
          $excel->getActiveSheet()->fromArray($data, NULL, "A$row");
          $data = array();
          $row++;
        }

        foreach ($q->{$answer_map} as $key => $property) {
          // Multichoice exception
          if ($q->type == 'multichoice') {
            $property['answer_format'] = $property['score_if_chosen'] > $property['score_if_not_chosen']
              ? QUIZ_EXPORT_CORRECT_ANSWER_MARK
              : '';
          }

          if (!empty($unset_keys)) {
            foreach ($unset_keys as $unset_key) {
              unset($property[$unset_key]);
            }
          }

          if ($strip_tags) {
            foreach ($property as $k => $val) {
              $property[$k] = trim(strip_tags(html_entity_decode($val, ENT_QUOTES | ENT_HTML401)));
            }
          }

          if (!empty($data)) {
            $data = array_merge($data, $property);
            $excel->getActiveSheet()->fromArray($data, NULL, "A$row");
            $data = array();
          }
          else {
            $excel->getActiveSheet()->fromArray($property, NULL, "D$row");
          }
          $row++;
        }
      }
      else {
        if ($q->type == 'truefalse' && $answer_map == 'correct_answer') {
          $q->correct_answer = $q->correct_answer ? t('True') : t('False');
        }

        $data = array_merge($data, array($q->{$answer_map}));
        $excel->getActiveSheet()->fromArray($data, NULL, "A$row");
        $row++;
      }
    }
  }

  return $excel;
}

/**
 * Helper tp get answers from scale question type.
 *
 * @param object $question
 *   Question object
 *
 * @return array
 */
function quiz_export_scale_answers($question) {
  $answers = array();
  foreach ($question as $key => $property) {
    if (is_numeric($key)) {
      $answers[] = array('answer' => $property->answer);
    }
  }

  return $answers;
}

/**
 * Helper to make answer properties clean.
 *
 * @param $answer_type_fields
 *   Current properties in answer.
 * @param string $quiz_type
 *   Current quiz type
 *
 * @return array
 *   Keys in properties should be removed.
 */
function quiz_export_clean_properties(&$answer_type_fields, $quiz_type) {
  $unset_keys = &drupal_static(__FUNCTION__ . $quiz_type);

  if (!empty($unset_keys)) {
    $answer_type_fields = array_diff($answer_type_fields, $unset_keys);
    return $unset_keys;
  }

  // Remove unnecessary fields like id & format.
  $unset_keys_id = array();
  array_walk($answer_type_fields, function ($key, $idx) use (&$unset_keys_id, $quiz_type) {
    if ($key === 'id' || $key === 'match_id'
      || strpos($key, 'score_if_') !== FALSE
      || (strpos($key, 'format') !== FALSE && ($quiz_type !== 'multichoice' || ($quiz_type === 'multichoice' && strpos($key, 'answer') === FALSE)))
      ) {
      $unset_keys_id[] = $idx;
    }
  });

  $unset_keys = [];
  foreach ($unset_keys_id as $id_key) {
    $unset_keys[] = $answer_type_fields[$id_key];
    unset($answer_type_fields[$id_key]);
  }

  return $unset_keys;
}
