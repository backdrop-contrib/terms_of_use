<?php
/**
 * @file
 * Page callbacks for the Terms of Use module.
 */

/**
 * Menu callback; show settings form.
 *
 * @see terms_of_use_admin_settings_validate()
 */
function terms_of_use_admin_settings() {
  $config = config('terms_of_use.settings');
  $form['#config'] = 'terms_of_use.settings';

  // Adding the fieldset for node specification.
  $form['terms_of_use_text'] = array(
    '#type' => 'fieldset',
    '#title' => t('Source text'),
  );
  $form['terms_of_use_text']['terms_of_use_node_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of content where Terms of Use are published'),
    '#default_value' => $config->get('terms_of_use_node_title'),
    '#autocomplete_path' => 'terms_of_use/autocomplete',
  );

  // Adding the fieldset for form specification.
  $form['terms_of_use_form'] = array(
    '#type' => 'fieldset',
    '#title' => t('Registration form display settings'),
  );
  $form['terms_of_use_form']['terms_of_use_fieldset_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Label for the fieldset'),
    '#default_value' => $config->get('terms_of_use_fieldset_name'),
    '#description' => t('The text for the Terms of Use and the [x] checkbox are contained in a fieldset. Type here the title for that fieldset.'),
  );
  $form['terms_of_use_form']['terms_of_use_checkbox_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label for the checkbox'),
    '#default_value' => $config->get('terms_of_use_checkbox_label'),
    '#description' => t('The token @link is available to insert a link to the Terms. Examples include: <em>I agree with the @link.</em>, <em>I agree with these terms.</em>, or <em>I CERTIFY THAT I AM OVER THE AGE OF 18 YEARS OLD.</em> '),
  );

  return system_settings_form($form);
}

/**
 * Validate the terms_of_use_admin_settings form.
 *
 * @see terms_of_use_admin_settings()
 */
function terms_of_use_admin_settings_validate($form, &$form_state) {
  if (isset($form_state['values']['terms_of_use_node_id'])) {
    $nid = $form_state['values']['terms_of_use_node_id'];
    if (empty($nid)) {
      form_set_error('terms_of_use_node_id', t('You must specify a node <em>nid</em>.'));
    }
    else {
      $node = node_load($nid);
      if ($node == FALSE) {
        form_set_error('terms_of_use_node_id', t('No post was published with <em>nid</em> !nid.', array('!nid' => $nid)));
      }
      else {
        variable_set('terms_of_use_node_title', $node->title);
      }
    }
  }
  elseif (!empty($form_state['values']['terms_of_use_node_title'])) {
    $nid = db_select('node', 'n')
      ->fields('n', array('nid'))
      ->condition('n.title', db_like($form_state['values']['terms_of_use_node_title']), 'LIKE')
      ->condition('n.status', 1)
      ->range(0, 1)
      ->addTag('node_access')
      ->execute()
      ->fetchField();

    if (!$nid) {
      form_set_error('terms_of_use_node_title', t('No post was published with this title.'));
    }
    else {
      variable_set('terms_of_use_node_id', $nid);
    }
  }
  else {
    form_set_error('terms_of_use_node_title', t('You must specify a node title.'));
  }
}

/**
 * Helper function for autocompletion.
 */
function terms_of_use_autocomplete($string = '') {
  $matches = array();
  if ($string != '') {
    $result = db_select('node', 'n')
      ->fields('n', array('nid', 'title'))
      ->condition('n.title', '%' . db_like($string) . '%', 'LIKE')
      ->condition('n.status', 1)
      ->range(0, 10)
      ->addTag('node_access')
      ->execute();

    foreach ($result as $node) {
      $matches[$node->title] = $node->title;
    }
  }

  backdrop_json_output($matches);
}
