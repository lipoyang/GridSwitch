/********** UIの要素 ***********/
// ボタン
const btn_connect  = document.getElementById('btn_connect'); // 接続
const btn_disconnect = document.getElementById('btn_disconnect');  // 切断
const btn_reload   = document.getElementById('btn_reload'); // Reload
const btn_save     = document.getElementById('btn_save');   // Save
const btn_import   = document.getElementById('btn_import'); // Import
const btn_export   = document.getElementById('btn_export'); // Export
// テキスト
const text_connect = document.getElementById('text_connect'); // 接続時のメッセージ
const text_command = [                      // コマンド割り当て
  document.getElementById('text_command_1A'),
  document.getElementById('text_command_2A'),
  document.getElementById('text_command_3A'),
  document.getElementById('text_command_4A'),
  document.getElementById('text_command_5A'),
  document.getElementById('text_command_1B'),
  document.getElementById('text_command_2B'),
  document.getElementById('text_command_3B'),
  document.getElementById('text_command_4B'),
  document.getElementById('text_command_5B')
];
// 表示領域
const panel_connect = document.getElementById('panel_connect'); // 接続画面
const panel_main    = document.getElementById('panel_main');    // メイン画面

/********** シリアル通信の定数・変数 ***********/
let comPort = null; // COMポート
const COM_READ     = 0x10; // READコマンド
const COM_WRITE    = 0x20; // WRITEコマンド
const ERR_CHECKSUM = 0x80; // エラー：チェックサムエラー
const ERR_COMMAND  = 0x81; // エラー：不正なコマンド
const ERR_PARAM    = 0x82; // エラー：不正なパラメータ

/********** コマンド設定データの定数・変数 ***********/
const COMMAND_MAX = 10; // 設定できるコマンドの数
const COMMAND_LEN = 32; // コマンドの最大文字数

// コマンドテーブル
let commandTable = [
  "", "", "", "", "",
  "", "", "", "", ""
];

/********** UIのイベントハンドラ ***********/
// 「接続」ボタン
btn_connect.addEventListener('click', async function () {
  try {
    // COMポートを開く
    comPort = await navigator.serial.requestPort();
    await comPort.open({ baudRate: 115200 });
    
    // コマンドテーブルの読み出し
    const result = await loadCommandTable();

    // 画面表示切替
    if(result){
      panel_connect.style.display = "none";
      panel_main.style.display = "block";
    }
  } catch (error) {
    error_toast("ERROR: " + error)
    comPort = null;
  }
});

// 「切断」ボタン
btn_disconnect.addEventListener('click', async function (){
  if(comPort != null){
    await comPort.close();
  }
  // 画面表示切替
  text_connect.innerText = "";
  panel_main.style.display = "none";
  panel_connect.style.display = "block";
});

// 「Reload」ボタン
btn_reload.addEventListener('click', async function ()
{
  await loadCommandTable();
});

// 「Save」ボタン
btn_save.addEventListener('click', async function ()
{
  await saveCommandTable();
});

// 「Import」ボタン
btn_import.addEventListener('click', async function (){

  // ファイルを読み込む (PC版 Chrome, Edge, Opera が対応)
  const options = {
    types: [{
      description: "JSON File",
      accept: {'text/json': ['.json']},
    }],
  };
  const handleList = await window.showOpenFilePicker(options);
  const file = await handleList[0].getFile(); // 1個目のファイル
  const json = await file.text();

  // JSON文字列をキーマップのデータに変換
  commandTable = JSON.parse(json);
  
  // 表示を更新
  showCommandTable();
});

// 「Export」ボタン
btn_export.addEventListener('click', async function (){

  // キーマップのデータをJSON文字列に変換
  const json = JSON.stringify(commandTable);

  // ファイルに保存 (PC版 Chrome, Edge, Opera が対応)
  const options = {
    suggestedName: 'CommandTable',
    types: [{
      description: 'JSON file',
      accept: {'text/json': ['.json']},
    }],
  };
  const handle = await window.showSaveFilePicker(options);
  const writable = await handle.createWritable();
  await writable.write(json);
  await writable.close();
});

/********** コマンドテーブルのUI処理 **********/

// コマンドテーブル → UI表示
function showCommandTable()
{
  for(let index = 0; index < COMMAN_MAX; index++){
    text_command[index].value = commandTable[index];
  }
}

// UI表示 → コマンドテーブル
function setCommandTable()
{
  for(let index = 0; index < COMMAN_MAX; index++){
    commandTable[index] = text_command[index].value;
  }
}

/********** キーマップのシリアル通信処理 ***********/

// コマンドテーブルのロード（デバイスから読み出し）
async function loadCommandTable()
{
  let commandTableTemp = new Array(); 
  let success = true;
  let errorIndex = -1;

  // 番号ごとにREADコマンドを実行
  for(let index = 0; index < COMMAN_MAX; index++){
    // コマンド送信
    const command = COM_READ;
    const data = [ index ];
    await sendCommand(command, data);
    // 応答受信
    const telegram = await recvResponse();
    if(telegram != false){
      // コマンドのチェック
      const command = telegram[3];
      if(command != COM_READ){
        switch(command){
          case ERR_CHECKSUM:
            error_toast("ERROR: デバイス側でチェックサムエラーがありました。");
            success = false; errorIndex = index;
            break;
          case ERR_COMMAND:
            error_toast("ERROR: デバイス側で不正なコマンドと判定されました。");
            success = false; errorIndex = index;
            break;
          case ERR_PARAM:
            error_toast("ERROR: デバイス側で不正なパラメータと判定されました。");
            success = false; errorIndex = index;
          break;
            default:
            error_toast("ERROR: 未定義のエラーを受信しました。");
            success = false; errorIndex = index;
            break;
        }
      }
      // 番号のチェック
      const recvIndex    = telegram[4];
      if(index != recvIndex){
        error_toast("ERROR: 番号が一致しません。 " + index + " / " + recvIndex);
        success = false; errorIndex = index;
        break;
      }
      // 受信データの解釈
      const bData = BytesTrim( telegram.slice(5, 5 + COMMAND_LEN) );
      commandTableTemp[index] = new TextDecoder().decode(bData);
    }else{
      // 応答受信失敗
      success = false; errorIndex = index;
      break;
    }
  }
  // 全部のREADが成功したらデータ更新
  if(success){
    // コマンドテーブルを更新
    commandTable = commandTableTemp;
    // 表示を更新
    showCommandTable();

    show_toast("コマンドテーブルを読み出しました。");
    return true;
  }else{
    error_toast("ERROR: 番号 " + errorIndex + " でエラーがありました。処理を中断します。");
    return false;
  }
}

// キーマップのセーブ (デバイスへ書き込み)
async function saveCommandTable()
{
  let success = true;
  let errorIndex = -1;

  // コマンドテーブルを更新
  setCommandTable();

  // 番号ごとにWRITEコマンドを実行
  for(let index = 0; index < COMMAN_MAX; index++){
    // 送信データの生成
    const bData = new TextEncoder().encode(commandTable[index]);
    const data = new Uint8Array(1 + COMMAND_LEN);
    data[0] = index;
    for(let i = 0; i < bData.length; i++) data[1 + i] = bData[i];
    // コマンド送信
    const command = COM_WRITE;
    await sendCommand(command, data);
    // 応答受信
    const telegram = await recvResponse();
    if(telegram != false){
      // 応答のチェック
      const response = telegram[3];
      if(response != COM_WRITE){
        switch(response){
          case ERR_CHECKSUM:
            error_toast("ERROR: デバイス側でチェックサムエラーがありました。");
            success = false; errorIndex = index;
            break;
          case ERR_COMMAND:
            error_toast("ERROR: デバイス側で不正なコマンドと判定されました。");
            success = false; errorIndex = index;
            break;
          case ERR_PARAM:
            error_toast("ERROR: デバイス側で不正なパラメータと判定されました。");
            success = false; errorIndex = index;
            break;
          default:
            error_toast("ERROR: 未定義のエラーを受信しました。");
            success = false; errorIndex = index;
            break;
        }
      }
      // 番号のチェック
      const recvIndex    = telegram[4];
      if(index != recvIndex){
        error_toast("ERROR: レイヤー番号が一致しません。 " + index + " / " + recvIndex);
        success = false; errorIndex = index;
        break;
      }
    }else{
      // 応答受信失敗
      success = false; errorIndex = index;
      break;
    }
  }
  if(success){
    show_toast("コマンドテーブルを書き込みました。");
  }else{
    error_toast("ERROR: 番号 " + errorIndex + " でエラーがありました。処理を中断します。");
  }
}

// 配列をゼロ終端までに切り詰める
function BytesTrim(data)
{
  let end = 0;
  for(end = 0; end < COMMAND_LEN; end++){
    if(data[end] == 0x00) break;
  }
  if (end == 0) return new Uint8Array(0);
  return data.slice(0,end);
}

/********** シリアル通信処理 (コマンド送信と応答受信) ***********/

// コマンドの送信
async function sendCommand(command, data)
{
  // 電文の生成
  const telegram =  new Uint8Array(data.length + 5);
  telegram[0] = 0xAA;
  telegram[1] = 0x55;
  telegram[2] = data.length + 2;
  telegram[3] = command;
  //    データ
  for (let i = 0; i < data.length; i++) {
    telegram[4 + i] = data[i];
  }
  //    チェックサム
  let sum = 0;
  for (let i = 0; i < telegram.length - 1; i++) {
    sum += telegram[i];
  }
  telegram[4 + data.length] = sum & 0xFF;

  // 電文の送信
  const writer = comPort.writable.getWriter();
  await writer.write(telegram);
  writer.releaseLock();
}

// コマンド応答の受信
async function recvResponse()
{
  let isError  = false; // エラーか？
  let received = false; // 電文受信成功したか？
  let recvCnt = 0;      // 電文の受信バイトカウント
  let len = 0;          // 電文の Len の値
  const telegram =  new Uint8Array(256); // 電文受信バッファ

  // COMポートが受信可能か？
  if (comPort.readable) {
    const reader = comPort.readable.getReader();
    try {
      // 電文受信完了かエラーまで
      while (true) {
        // COMポートからデータ受信 (タイムアウトあり)
        value = await read_with_timeout(reader, 1000);
        if (value == false) {
          isError = true; // タイムアウト
          break;
        }
        //console.log("Received " + value);
        
        // 受信したデータを1バイトずつ処理
        for(let i = 0; i < value.length; i++){
          const bData = value[i];
          telegram[recvCnt] = bData;
          
          // 1バイト目: AA
          if(recvCnt == 0){
            if(bData != 0xAA){
              isError = true;
              error_toast("ERROR: Header 1 not AA but " + bData.toString(16));
            }
          }
          // 2バイト目: 55
          else if(recvCnt == 1){
            if(bData != 0x55){
              isError = true;
              error_toast("ERROR: Header 1 not 55 but " + bData.toString(16));
            }
          }
          // 3バイト目: Len
          else if(recvCnt == 2){
            len = bData;
          }
          // 最後のバイト
          else if(recvCnt == len + 2){
            // チェックサム検証
            let sum = 0;
            for(let j = 0; j < len + 2; j++){ sum += telegram[j]; }
            sum = sum & 0xFF;
            if(sum != bData){
              isError = true;
              error_toast("ERROR: Check Sum " + sum.toString(16) + "!= " + bData.toString(16));
            }else{
              received = true;
            }
          }
          if (isError) break;
          if (received) break;
          recvCnt++;
        }
        if (isError) break;
        if (received) break;
      }
    } catch (error) {
      error_toast("ERROR: " + error);
    } finally {
      reader.releaseLock();
    }
  }else{
    error_toast("ERROR: COM Port not readable");
  }

  // 電文受信完了したか？
  if(received){
    return telegram; // 成功なら電文を返す
  }else{
    return false;    // 失敗ならfalseを返す
  }
}

// タイムアウト付きシリアル受信
async function read_with_timeout(reader, timeout)
{
  let reading = true; // 受信中フラグ

  // シリアル受信のプロミス
  const receivePromise = new Promise(async(resolve, reject) => {
    const { value, done } = await reader.read();
    reading = false; // 受信終了
    if (done) {
      reject("ERROR: No Data or Port Closed");
    } else{
      resolve(value);
    }
  });
  // タイムアウトのプロミス
  const timeoutPromise = new Promise((resolve, reject) => {
    setTimeout(() => {
      // readerをキャンセルすることでシリアル受信のプロミスも終わらせる
      if(reading) reader.cancel();
      reject("ERROR: Timeout")
    }, timeout);
  });
  // シリアル受信かタイムアウトかどちらかが終わるのを待つ
  try{
    const buff = await Promise.race([receivePromise, timeoutPromise]);
    return buff; // 受信成功
  } catch(error) {
    error_toast("ERROR: " + error);
    return false; // 受信失敗
  }
}

// トースト表示
function show_toast(message)
{
  console.log(message);

  const jsFrame = new JSFrame();
  jsFrame.showToast({
    html: message, align: 'top', duration: 3000
  });
}

// トースト表示(エラー)
function error_toast(message)
{
  console.log(message);

  const jsFrame = new JSFrame();
  jsFrame.showToast({
    html: message, align: 'top', duration: 5000
  });
}
